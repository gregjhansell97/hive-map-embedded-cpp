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

typedef unsigned char Type;
typedef unsigned char Hash;

//static unsigned int max_size = 255; // figure out more clever approach?

static const Type NONE = 0;

/**
 * All messages must start with a message header named header?
 */
struct Header {
    Type type; // id of the message
    unsigned char bcast_radius; // required broadcast
    loc::Id destination; // destination node
    unsigned char size; // size of message past header
    unsigned char hops_threshold; //
    Hash hash; // determine if message was sent already
};

struct Subscriber {
    Type type; 
    void(*callback)(void* msg);
};

} //msg

} //hmap

#endif // HIVE_MAP_CPP_TYPES_H_
