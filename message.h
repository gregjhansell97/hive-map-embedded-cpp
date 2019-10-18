#ifndef HIVE_MAP_CPP_TYPES_H_
#define HIVE_MAP_CPP_TYPES_H_

#include <stddef.h>

namespace hmap {

namespace loc {

typedef unsigned char Id;

static const Id ANY = 1; // any location
static const Id NONE = 0; // no location (anonymous)
static const Id ERROR = 2; // not sure yet lol

} //loc

namespace msg {

typedef unsigned char Id;
typedef unsigned char Hash;

/**
 * All messages must start with a message header named header?
 */
struct Header {
    Id id; // id of the message
    loc::Id destination; // destination node
    unsigned char bcast_radius; // required broadcast
    size_t size; // size of message past header
    Hash hash; // determine if message was sent already
};

} //msg

} //hmap

#endif // HIVE_MAP_CPP_TYPES_H_
