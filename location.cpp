#include <string.h>

#include "location.h"

namespace hmap {

void Location::add_channel(Channel& channel) {
    Channel** new_channels = new Channel* [m_chnl_len + 1];
    for(size_t i = 0; i < m_chnl_len; ++i) {
        new_channels[i] = m_channels[i]; // move over channels to new memory
    }
    delete [] m_channels;
    m_channels = new_channels;
    
    m_channels[m_chnl_len] = &channel;
    ++m_chnl_len;

    // need to go through destinations an update length for destination
    for(size_t i = 0; i < m_d_len; ++i) {
        m_destinations[i]->m_c_len = m_chnl_len;
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
    delete [] m_destinations;
    m_destinations = new_destinations;

    m_destinations[m_d_len] = new Destination(id, -1, m_channels, m_chnl_len);
    ++m_d_len;
}

void Location::deliver(void* data) {
    msg::Header* h = static_cast<msg::Header*>(data);
    for(size_t i = 0; i < m_cb_len; ++i) {
        msg::Callback& cb = *m_callbacks[i];
        if(cb.id == h->id) {
            (*cb.callback)(static_cast<char*>(data));
        }
    }
}

void Location::broadcast(void* data) {
    msg::Header* h = static_cast<msg::Header*>(data);
    bool match = false;// look for destination match
    for(size_t i = 0; i < m_d_len; ++i) {
        if(m_destinations[i]->m_id == h->id) {
            match = true;
            break;
        }
    }
    // not in the destination, doc off a bcast_radius
    if(!match && h->bcast_radius == 0) {
        return; // now match and no need to broadcast
    } else if(!match && h->bcast_radius > 0) {
        --h->bcast_radius;
    }
    for(size_t j = 0; j < m_chnl_len; ++j) {
        Channel& c = *m_channels[j];
        c.write(static_cast<char*>(data), h->size);
    }
}

void Location::cycle() {
    // read for data first
    msg::Header h;
    for(size_t i = 0; i < m_chnl_len; ++i) {
        Channel& c = *m_channels[i];
        if(c.read(&h) == 0) continue; // nothing read
        char* data = new char[h.size];
        memcpy(data, &h, sizeof(h));
        c.read(data + sizeof(h), h.size - sizeof(h)); 

        if(h.destination == m_id) {
            deliver(data);
        } else {
            broadcast(data);
        }
    }

}

} // hmap
