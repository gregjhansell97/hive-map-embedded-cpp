#ifndef HIVE_MAP_CPP_LOCATION_H_
#define HIVE_MAP_CPP_LOCATION_H_

#include <stddef.h>

#include "destination.h"
#include "message.h"

namespace hmap {

class Location {
public:
    Location(const loc::Id id):
        m_d_len(0),
        m_chnl_len(0),
        m_cb_len(0),
        m_id(id) { };
    

    void add_channel(Channel& channel);

    template<typename T>
    void subscribe(void(*cb)(void* msg)) {
        T instance; //hacky but need to get id
        const msg::Id msg_id = instance.header.id;

        msg::Callback** new_callbacks = new msg::Callback* [m_cb_len + 1];
        for(size_t i = 0; i < m_cb_len; ++i) {
            new_callbacks[i] = m_callbacks[i]; // copy over old callbacks
        }
        delete [] m_callbacks;
        m_callbacks = new_callbacks;
        
        m_callbacks[m_cb_len]->id = msg_id;
        m_callbacks[m_cb_len]->callback = cb;
        ++m_cb_len;
    }

    void deliver(void* data);

    void broadcast(void* data);

    void cycle();

    Destination& destinations(const loc::Id id);

private:
    const loc::Id m_id;

    Destination** m_destinations;
    size_t m_d_len;

    Channel** m_channels;
    size_t m_chnl_len;

    msg::Callback** m_callbacks;
    size_t m_cb_len;
};

} // hmap

#endif // HIVE_MAP_CPP_LOCATION_H_
