#ifndef HIVE_MAP_EMBEDDED_CPP_END_POINT_H_
#define HIVE_MAP_EMBEDDED_CPP_END_POINT_H_

#include <stddef.h>

#include "message.h"

#define MSGS_RCVD_BUFFER 20;

namespace hmap {

namespace network {

namespace non_blocking {

/**
 * THOUGHTS:
 *
 * - read and write are non-blocking (for now). though there needs to be an
 *   options, and if there is an option then the system needs to be
 *   multithreaded
 */

class EndPoint {
friend class Location;
friend class Destination;
public:
    /**
     * Writes "len" many bytes to the channel
     *
     * Args:
     *     data(char*): array of bytes being written
     *     len(size_t): potential size of character array
     */
    virtual void broadcast(char* data, size_t len) = 0;

    /**
     * Corresponding read to write function. A pointer to a pre-allocated array
     * is provided. The pre-allocated array is of at least size "len". If no
     * data is read the return value is 0
     *
     * Args:
     *     data(char*): an array of bytes of size "len" or higher.
     *         the chacter array is expected to be overwritten with new
     *         information
     *     len(size_t): size of character array provided
     *
     * Returns:
     *     (size_t): number of bytes read (at most len)
     */
    virtual size_t deliver(char* data, const size_t len) = 0;
};

} //non_blocking

namespace blocking {

    //TODO create blocking endpoint
} // blocking

} //network

} // hmap

#endif // HIVE_MAP_CPP_END_POINT_H_
