#include "channel.h"

namespace hmap {

size_t Channel::read(msg::Header* h) {
    this->read(static_cast<char*>(static_cast<void*>(h)), sizeof(msg::Header));
    // look at header and if its on ringbuffer already suck in rest of message
    // and return 0, otherwise return size
}


} // hmap
