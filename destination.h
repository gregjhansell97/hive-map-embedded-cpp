#ifndef HIVE_MAP_CPP_DESTINATION_H_
#define HIVE_MAP_CPP_DESTINATION_H_

#include <stddef.h>

#include "channel.h"
#include "message.h"

namespace hmap {

class Destination {
friend class Location;
public:
    Destination(
            const loc::Id id,
            unsigned char hops,
            Channel** channels,
            size_t c_len):
        m_id(id),
        m_hops(hops),
        m_channels(channels),
        m_c_len(c_len) {};
    
    /**
     * Publishes a message to all channels
     *
     * Args:
     *     message(T): message (should be struct) that had a header attribute.
     *         message will be published
     */
    template<typename T>
    void publish(T& message) {
        msg::Header* header = (msg::Header*)(&message);
        header->destination = m_id;
        header->size = sizeof(message);
        for(size_t i = 0; i < m_c_len; ++i) {
            m_channels[i]->write(header);
        }
    }

private:
    const loc::Id m_id;
    unsigned char m_hops;
    Channel** m_channels;
    size_t m_c_len;
};

} // hmap

#endif // HIVE_MAP_CPP_LOCATION_H_
