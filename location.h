#ifndef HIVE_MAP_EMBEDDED_CPP_LOCATION_H_
#define HIVE_MAP_EMBEDDED_CPP_LOCATION_H_

#include <stddef.h>

#include "destination.h"
#include "message.h"
#include "end_point.h"

#define HOPS_MSG 5

namespace hmap {

struct HopsMsg {
    msg::Header header {
        .type = HOPS_MSG,
        .bcast_radius = 1,
        .destination = loc::ANY,
        .size = sizeof(HopsMsg)
    };
    loc::Id loc;
    unsigned char hops_away;
};

class Location {
public:
    Location(const loc::Id id):
        id_(id),
        d_len_(0),
        end_points_len_(0),
        sub_len_(0) { };

    ~Location();

    void bind(network::non_blocking::EndPoint& endpoint);

    template<typename T>
    void subscribe(void(*cb)(void* msg)) {
        const msg::Type msg_type = T::type;

        msg::Subscriber** subs = new msg::Subscriber*[sub_len_ + 1];
        for(size_t i = 0; i < sub_len_; ++i) {
            subs[i] = subscribers_[i]; // copy over old subscriptions
        }
        if(sub_len_ > 0) delete [] subscribers_; // frees old
        subscribers_ = subs;
        
        subscribers_[sub_len_] = new msg::Subscriber;
        subscribers_[sub_len_]->type = msg_type;
        subscribers_[sub_len_]->callback = cb;
        ++sub_len_;
    }

    void deliver(void* data);

    void broadcast(void* data);

    // TODO get rid of/make improve
    void update_destinations(void* data);

    void cycle();

    Destination& destinations(const loc::Id id);

private:
    const loc::Id id_;

    Destination** destinations_;
    size_t d_len_;

    network::non_blocking::EndPoint** end_points_;
    size_t end_points_len_;

    msg::Subscriber** subscribers_;
    size_t sub_len_;
};

} // hmap

#endif // HIVE_MAP_EMBEDDED_CPP_LOCATION_H_
