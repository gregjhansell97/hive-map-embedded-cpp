#include "channel.h"

namespace hmap {


void Channel::send_msg(msg::Header* h) {
    this->send_data(static_cast<char*>(static_cast<void*>(h)), h->size);
}

size_t Channel::recv_header(msg::Header* h) {
    return this->recv_data(
            static_cast<char*>(static_cast<void*>(h)), sizeof(msg::Header));
    // look at header and if its on ringbuffer already suck in rest of message
    // and return 0, otherwise return size
}


} // hmap
