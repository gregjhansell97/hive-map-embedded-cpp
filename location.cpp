#include <string.h>

#include "location.h"

namespace hmap {


Location::~Location() {
    /*
    for(size_t i = 0; i < m_d_len; ++i) {
        delete m_destinations[i];
    }
    if(m_d_len > 0) delete [] m_destinations;
    for(size_t i = 0; i < m_sub_len; ++i) {
        delete m_subscribers[i];
    }
    if(m_sub_len > 0) delete []  m_subscribers;
    if(m_chnl_len > 0) delete [] m_channels; 
    */
}

void Location::bind(network::non_blocking::EndPoint& end_point) {
    /*
    Channel** new_channels = new Channel* [m_chnl_len + 1];
    for(size_t i = 0; i < m_chnl_len; ++i) {
        new_channels[i] = m_channels[i]; // move over channels to new memory
    }
    if(m_chnl_len > 0) delete [] m_channels;
    m_channels = new_channels;
    
    m_channels[m_chnl_len] = &channel;
    ++m_chnl_len;

    // need to go through destinations an update length for destination
    for(size_t i = 0; i < m_d_len; ++i) {
        m_destinations[i]->m_c_len = m_chnl_len;
        m_destinations[i]->m_channels = m_channels;
    }
    */
}

Destination& Location::destinations(const loc::Id id) {
    return *(new Destination(id, -1, nullptr, 0));
    /*
    for(size_t i = 0; i < m_d_len; ++i) {
        if(m_destinations[i]->m_id == id) {
            return *(m_destinations[i]);
        }
    } // must be a new destination
    Destination** new_destinations = new Destination* [m_d_len];
    for(size_t i = 0; i < m_d_len; ++i) {
        new_destinations[i] = m_destinations[i];
    }
    if(m_d_len > 0) delete [] m_destinations;
    m_destinations = new_destinations;

    m_destinations[m_d_len] = new Destination(id, -1, m_channels, m_chnl_len);
    ++m_d_len;
    return *m_destinations[m_d_len - 1];
    */
}

void Location::deliver(void* data) {
/*
    msg::Header& meta = *static_cast<msg::Header*>(data);
    // goes through all callbacks
    for(size_t i = 0; i < m_sub_len; ++i) {
        msg::Subscriber& s = *m_subscribers[i];
        if(s.type == meta.type) { // matches subscription
            (*s.callback)(static_cast<char*>(data));
        }
    }
*/
}

void Location::broadcast(void* data) {
/*
    msg::Header& h_msg = *static_cast<msg::Header*>(data); // get header info
    bool match = false;// look for destination match, and closer
    for(size_t i = 0; i < m_d_len; ++i) {
        Destination& d = *m_destinations[i];
        // looking for common destination thats closer than sender of msg
        if(d.m_id == h_msg.destination && 
                d.m_hops < h_msg.hops_threshold) {
            // found it, adjust threshold
            h_msg.hops_threshold = d.m_hops - 1;
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
    for(size_t j = 0; j < m_chnl_len; ++j) {
        Channel& c = *m_channels[j];
        c.send_data(static_cast<char*>(data), h_msg.size);
    }
*/
}

/*
void Location::update_destinations(void* data) {

    HopsMsg& h_msg = *static_cast<HopsMsg*>(data); // cast to hops message 
    for(size_t i = 0; i < m_d_len; ++i) { // go through destinations
        // go through destinations
        Destination& d = *m_destinations[i];
        if(d.m_id == h_msg.loc) { // hops to destination (.loc_id?
            if(h_msg.hops_away != -1 && 
                    h_msg.hops_away + 1 < d.m_hops) {
                // if closest destination goes away, shit outa luck...
                d.m_hops = h_msg.hops_away + 1;
            }
        }
    }
}
*/
void Location::cycle() {
    /*
    // goes through all channels and for reads info
    // exauhstive receive
    char* data = new char[hmap::msg::max_size];
    bool restart = false;
    for(size_t i = 0; i < m_chnl_len; ++i) {
        data[0] = msg::NONE; // if doesn't change, no message read
        Channel& c = *m_channels[i];
        c.recv_data(data, hmap::msg::max_size);
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
        } else if(meta.destination == m_id || 
                meta.destination == loc::ANY) { 
            // right location to deliver message (or destination is to any)
            deliver(data);
        } else {
            broadcast(data); // not right location send to other locatoins
        }
        if(i == m_chnl_len - 1 && restart) {
            i = 0;
        }
    }
    delete [] data; // no memory leaks

    // publishes hops messages
    HopsMsg h_msg;
    h_msg.loc = m_id;
    h_msg.hops_away = 0;
    for(size_t i = 0; i < m_chnl_len; ++i) {
        // send to all channels
        Channel& c = *m_channels[i];
        c.send_data(
            static_cast<char*>(static_cast<void*>(&h_msg)), 
            sizeof(HopsMsg));
    }
    for(size_t i = 0; i < m_d_len; ++i) { 
        // go through destinations
        Destination& d = *m_destinations[i];
        // messages are how many hops location thinks it is from destinations
        h_msg.loc = d.m_id;
        h_msg.hops_away = d.m_hops;
        for(size_t j = 0; j < m_chnl_len; ++j) {
            Channel& c = *m_channels[j];
            c.send_data(
                static_cast<char*>(static_cast<void*>(&h_msg)), 
                sizeof(HopsMsg));
        }
    }
    */
}

} // hmap
