#include <string.h>

#include "location.h"

namespace hmap {


Location::~Location() {
    // delete the destinations allocated
    for(size_t i = 0; i < d_len_; ++i) {
        delete destinations_[i];
    }
    if(d_len_ > 0) delete [] destinations_;
    // delete subscribers
    for(size_t i = 0; i < sub_len_; ++i) {
        delete subscribers_[i];
    }
    if(sub_len_ > 0) delete []  subscribers_;
    if(end_points_len_ > 0) delete [] end_points_; 
}

void Location::bind(network::non_blocking::EndPoint& end_point) {
    typedef network::non_blocking::EndPoint EndPoint;
    EndPoint** new_end_points = new EndPoint* [end_points_len_ + 1];
    for(size_t i = 0; i < end_points_len_; ++i) {
        new_end_points[i] = end_points_[i]; // move over end points to new memory
    }
    if(end_points_len_ > 0) delete [] end_points_;
    end_points_ = new_end_points;
    
    end_points_[end_points_len_] = &end_point;
    ++end_points_len_;

    // need to go through destinations an update length for destination
    // TODO perhaps a templated data structure (like a light weight vector)
    for(size_t i = 0; i < d_len_; ++i) {
        destinations_[i]->end_points_len_ = end_points_len_;
        destinations_[i]->end_points_ = end_points_;
    }
}

Destination& Location::destinations(const loc::Id id) {
    for(size_t i = 0; i < d_len_; ++i) {
        if(destinations_[i]->id_ == id) {
            return *(destinations_[i]);
        }
    } // must be a new destination
    Destination** new_destinations = new Destination* [d_len_];
    for(size_t i = 0; i < d_len_; ++i) {
        new_destinations[i] = destinations_[i];
    }
    if(d_len_ > 0) delete [] destinations_;
    destinations_ = new_destinations;

    destinations_[d_len_] = new Destination(
            id, 
            -1, 
            end_points_, 
            end_points_len_);
    ++d_len_;
    return *destinations_[d_len_ - 1];
}

void Location::deliver(void* data) {
    msg::Header& meta = *static_cast<msg::Header*>(data);
    // goes through all callbacks
    for(size_t i = 0; i < sub_len_; ++i) {
        msg::Subscriber& s = *subscribers_[i];
        if(s.type == meta.type) { // matches subscription
            (*s.callback)(static_cast<char*>(data));
        }
    }
}

void Location::broadcast(void* data) {
    typedef network::non_blocking::EndPoint EndPoint;
    msg::Header& h_msg = *static_cast<msg::Header*>(data); // get header info
    bool match = false;// look for destination match, and closer
    for(size_t i = 0; i < d_len_; ++i) {
        Destination& d = *destinations_[i];
        // looking for common destination thats closer than sender of msg
        if(d.id_ == h_msg.destination && 
                d.hops_ < h_msg.hops_threshold) {
            // found it, adjust threshold
            h_msg.hops_threshold = d.hops_ - 1;
            match = true;
            break;
        }
    }
    if(!match && h_msg.bcast_radius == 0) {
        return; // now match and no need to broadcast
    } else if(!match && h_msg.bcast_radius > 0) {
    // not in the destination, doc off a bcast_radius
        --(h_msg.bcast_radius);
    }
    // send over all channels
    for(size_t j = 0; j < end_points_len_; ++j) {
        EndPoint& e = *end_points_[j];
        e.broadcast(static_cast<char*>(data), h_msg.size);
    }
}

void Location::update_destinations(void* data) {
    HopsMsg& h_msg = *static_cast<HopsMsg*>(data); // cast to hops message 
    for(size_t i = 0; i < d_len_; ++i) { // go through destinations
        // go through destinations
        Destination& d = *destinations_[i];
        if(d.id_ == h_msg.loc) { // hops to destination (.loc_id?
            if(h_msg.hops_away != -1 && 
                    h_msg.hops_away + 1 < d.hops_) {
                // if closest destination goes away, shit outa luck...
                d.hops_ = h_msg.hops_away + 1;
            }
        }
    }
}

void Location::cycle() {
    typedef network::non_blocking::EndPoint EndPoint;
    // goes through all channels and for reads info
    // exauhstive receive
    char data[MAX_MESSAGE_SIZE];
    bool restart = false;

    for(size_t i = 0; i < end_points_len_; ++i) {
        data[0] = msg::NONE; // if doesn't change, no message read
        EndPoint& e = *end_points_[i];
        e.deliver(data, MAX_MESSAGE_SIZE);
        if((unsigned char)data[0] == msg::NONE) { // leading index is id
            continue; // nothing read
        } else {
            restart = true;
        }
        msg::Header& meta = *static_cast<msg::Header*>(
                static_cast<void*>(data));
        // now has all info needed
        if(meta.type == HOPS_MSG) {
            update_destinations(data); // changes hops table
        } else if(meta.destination == id_ || 
                meta.destination == loc::ANY) { 
            // right location to deliver message (or destination is to any)
            deliver(data);
        } else {
            broadcast(data); // not right location send to other locatoins
        }
        if(i == end_points_len_ - 1 && restart) {
            i = 0;
        }
    }

    // publishes hops messages
    HopsMsg h_msg;
    h_msg.loc = id_;
    h_msg.hops_away = 0;
    for(size_t i = 0; i < end_points_len_; ++i) {
        // send to all end point
        EndPoint& e = *end_points_[i];
        e.broadcast(
            static_cast<char*>(static_cast<void*>(&h_msg)), 
            sizeof(HopsMsg));
    }
    for(size_t i = 0; i < d_len_; ++i) { 
        // go through destinations
        Destination& d = *destinations_[i];
        // messages are how many hops location thinks it is from destinations
        h_msg.loc = d.id_;
        h_msg.hops_away = d.hops_;
        for(size_t j = 0; j < end_points_len_; ++j) {
            EndPoint& e = *end_points_[j];
            e.broadcast(
                static_cast<char*>(static_cast<void*>(&h_msg)), 
                sizeof(HopsMsg));
        }
    }
}

} // hmap
