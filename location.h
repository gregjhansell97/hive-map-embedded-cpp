#ifndef HIVE_MAP_CPP_LOCATION_H_
#define HIVE_MAP_CPP_LOCATION_H_

#include <stddef.h>

#include "destination.h"
#include "message.h"
#include "end_point.h"

namespace hmap {

class Location {
public:
    Location(const loc::Id id):
        id_(id),
        d_len_(0),
        endpoints_len_(0),
        sub_len_(0) { };

    ~Location();

    void bind(network::non_blocking::EndPoint& endpoint);

    template<typename T>
    void subscribe(void(*cb)(void* msg)) {
        /*
        const msg::Type msg_type = T::type;

        msg::Subscriber** subs = new msg::Subscriber*[m_sub_len + 1];
        for(size_t i = 0; i < m_sub_len; ++i) {
            subs[i] = m_subscribers[i]; // copy over old subscriptions
        }
        if(m_sub_len > 0) delete [] m_subscribers; // frees old
        m_subscribers = subs;
        
        m_subscribers[m_sub_len] = new msg::Subscriber;
        m_subscribers[m_sub_len]->type = msg_type;
        m_subscribers[m_sub_len]->callback = cb;
        ++m_sub_len;*/
    }

    void deliver(void* data);

    void broadcast(void* data);

    //void update_destinations(void* data);

    void cycle();

    Destination& destinations(const loc::Id id);

private:
    const loc::Id id_;

    Destination** destinations_;
    size_t d_len_;

    network::non_blocking::EndPoint** endpoints_;
    size_t endpoints_len_;

    msg::Subscriber** subscribers_;
    size_t sub_len_;
};

} // hmap

#endif // HIVE_MAP_CPP_LOCATION_H_
