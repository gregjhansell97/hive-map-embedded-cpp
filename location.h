#ifndef HIVE_MAP_CPP_LOCATION_H_
#define HIVE_MAP_CPP_LOCATION_H_

#include <stddef.h>

#include "destination.h"
#include "message.h"

namespace hmap {

class Location {
public:
    Location(const loc::Id id):
        m_id(id),
        m_d_len(0),
        m_chnl_len(0),
        m_sub_len(0) { };
    

    void add_channel(Channel& channel);

    template<typename T>
    void subscribe(void(*cb)(void* msg)) {
        T instance; //hacky but need to get id
        const msg::Type msg_type = instance.header.type;

        msg::Subscriber** subs = new msg::Subscriber*[m_sub_len + 1];
        for(size_t i = 0; i < m_sub_len; ++i) {
            subs[i] = m_subscribers[i]; // copy over old subscriptions
        }
        if(m_sub_len > 0) delete [] m_subscribers; // frees old
        m_subscribers = subs;
        
        m_subscribers[m_sub_len] = new msg::Subscriber;
        m_subscribers[m_sub_len]->type = msg_type;
        m_subscribers[m_sub_len]->callback = cb;
        ++m_sub_len;
    }

    void deliver(void* data);

    void broadcast(void* data);

    void update_destinations(void* data);

    void cycle();

    Destination& destinations(const loc::Id id);

private:
    const loc::Id m_id;

    Destination** m_destinations;
    size_t m_d_len;

    Channel** m_channels;
    size_t m_chnl_len;

    msg::Subscriber** m_subscribers;
    size_t m_sub_len;
};

} // hmap

#endif // HIVE_MAP_CPP_LOCATION_H_
