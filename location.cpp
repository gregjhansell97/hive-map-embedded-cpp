#include <string.h>

#include "location.h"
#include "location_messages.h"

namespace hmap {

Location::~Location() {
    for(size_t i = 0; i < m_d_len; ++i) {
        delete m_destinations[i];
    }
    if(m_d_len > 0) delete [] m_destinations;
    for(size_t i = 0; i < m_sub_len; ++i) {
        delete m_subscribers[i];
    }
    if(m_sub_len > 0) delete []  m_subscribers;
}

void Location::add_channel(Channel& channel) {
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
}

Destination& Location::destinations(const loc::Id id) {
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
}

void Location::deliver(void* data) {
    msg::Header* h = static_cast<msg::Header*>(data);
    // goes through all callbacks
    for(size_t i = 0; i < m_sub_len; ++i) {
        msg::Subscriber& s = *m_subscribers[i];
        if(s.type == h->type) { // matches subscription
            (*s.callback)(static_cast<char*>(data));
        }
    }
}

void Location::broadcast(void* data) {
    msg::Header* h = static_cast<msg::Header*>(data); // get header info
    bool match = false;// look for destination match, and closer
    for(size_t i = 0; i < m_d_len; ++i) {
        // looking for matching destination thats closer than sender of msg
        if(m_destinations[i]->m_id == h->destination && 
                m_destinations[i]->m_hops < h->hops_threshold) {
            // found it, adjust threshold
            h->hops_threshold = m_destinations[i]->m_hops - 1;
            match = true;
            break;
        }
    }
    if(!match && h->bcast_radius == 0) {
        return; // now match and no need to broadcast
    } else if(!match && h->bcast_radius > 0) {
    // not in the destination, doc off a bcast_radius
        --h->bcast_radius;
    }
    // send over all channels
    for(size_t j = 0; j < m_chnl_len; ++j) {
        Channel& c = *m_channels[j];
        c.send_data(static_cast<char*>(data), h->size);
    }
}

void Location::update_destinations(void* data) {
    HopsMsg* h = static_cast<HopsMsg*>(data); // cast to hops message 
    for(size_t i = 0; i < m_d_len; ++i) { // go through destinations
        if(m_destinations[i]->m_id == h->loc) { // hops to destination
            if(h->hops_away != -1 && 
                    h->hops_away + 1 < m_destinations[i]->m_hops) {
                // if closest destination goes away, shit outa luck...
                //if(m_id == 33) cout << "Here" << endl;
                m_destinations[i]->m_hops = h->hops_away + 1;
            }
        }
    }
}

void Location::cycle() {
    // read for data first
    msg::Header h;
    // goes through all channels and for reads info
    // exauhstive receive
    for(size_t i = 0; i < m_chnl_len; ++i) {
        Channel& c = *m_channels[i];
        if(c.recv_header(&h) == 0) {
            continue; // nothing read
        } else { // something read go back to begining
            i = -1;
        }

        char* data = new char[h.size]; // allocate memory for msg size
        memcpy(data, &h, sizeof(h)); //copy over header information
        c.recv_data(data + sizeof(h), h.size - sizeof(h)); //reads rest of info
        // now has all info needed
        if(h.type == HOPS_MSG) {
            update_destinations(data); // changes hops table
        } else if(h.destination == m_id || 
                m_id == loc::ANY) { // right location to deliver message
            deliver(data);
        } else {
            broadcast(data); // not right location send to other locatoins
        }
        delete [] data; // no memory leaks
    }
    // publishes hops messages
    HopsMsg msg;
    msg.loc = m_id;
    msg.hops_away = 0;
    for(size_t j = 0; j < m_chnl_len; ++j) {
        Channel& c = *m_channels[j];
        c.send_data(
            static_cast<char*>(static_cast<void*>(&msg)), sizeof(HopsMsg));
    }
    for(size_t i = 0; i < m_d_len; ++i) { // go through destinations
        Destination& d = *m_destinations[i];
        msg.loc = d.m_id;
        msg.hops_away = d.m_hops;
        for(size_t j = 0; j < m_chnl_len; ++j) {
            Channel& c = *m_channels[j];
            c.send_data(
                static_cast<char*>(static_cast<void*>(&msg)), sizeof(HopsMsg));
        }
    }

}

} // hmap
