#ifndef HIVE_MAP_EMBEDDED_CPP_DESTINATION_H_
#define HIVE_MAP_EMBEDDED_CPP_DESTINATION_H_

#include <stddef.h>

#include "end_point.h"
#include "message.h"

namespace hmap {

class Destination {
friend class Location;
public:
    Destination(
            const loc::Id id,
            unsigned char hops,
            network::non_blocking::EndPoint** end_points,
            size_t e_len):
        id_(id),
        hops_(hops),
        end_points_(end_points),
        end_points_len_(e_len) {};
    
    /**
     * Publishes a message to all channels
     *
     * Args:
     *     message(T): message (should be struct) that had a header attribute.
     *         message will be published
     */
    template<typename T>
    void publish(T& msg) {
        // set up meta data to send
        msg.header.destination = id_;
        msg.header.size = sizeof(T);
        msg.header.hops_threshold = hops_;
        if(end_points_len_ > 0) {
            //msg.header.hash = m_channels[0]->random();
        }
        for(size_t i = 0; i < end_points_len_; ++i) {
            end_points_[i]->broadcast(
                    static_cast<char*>(static_cast<void*>(&msg)), sizeof(T));
        }
    }

private:
    const loc::Id id_;
    unsigned char hops_;
    network::non_blocking::EndPoint** end_points_;
    size_t end_points_len_;
};

} // hmap

#endif // HIVE_MAP_CPP_EMBEDDED_DESTINATION_H_
