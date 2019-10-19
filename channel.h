#ifndef HIVE_MAP_CPP_CHANNEL_H_
#define HIVE_MAP_CPP_CHANNEL_H_

#include <stddef.h>
#include <iostream>

#include "message.h"

#define MSGS_RCVD_BUFFER 20

using std::cout;
using std::endl;

namespace hmap {

/**
 * THOUGHTS:
 *
 * - read and write are non-blocking (for now). though there needs to be an
 *   options, and if there is an option then the system needs to be
 *   multithreaded
 */

class Channel {
friend class Location;
friend class Destination;
public:
    /**
     * Returns a random unsigned long
     */
    virtual unsigned long random() = 0;

    /**
     * Writes "len" many bytes to the channel
     *
     * Args:
     *     data(char*): array of bytes being written
     *     len(size_t): potential size of character array
     */
    virtual void send_data(char* data, size_t len) = 0;

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
    virtual size_t recv_data(char* data, const size_t len) = 0;
private:
    void send_msg(msg::Header* msg);
    size_t recv_header(msg::Header* h);
    void cycle();

    // received buffer (drops already received messages)
    msg::Hash m_msgs_rcvd[MSGS_RCVD_BUFFER];
    unsigned char m_msgs_rcvd_len; 
};

}

#endif // HIVE_MAP_CPP_CHANNEL_H_
