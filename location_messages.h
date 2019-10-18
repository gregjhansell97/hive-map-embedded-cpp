#ifndef HIVE_MAP_CPP_LOCATION_MESSAGES_H_
#define HIVE_MAP_CPP_LOCATION_MESSAGES_H_

#include <stddef.h>

#include "message.h"

#define HOPS_MSG_ID 11

namespace hmap {

struct HopsMsg {
    msg::Header header {
        .id = HOPS_MSG_ID,
        .destination = loc::ANY,
        .bcast_radius = 1, // can go one hop with-out it dieing
    };
    const loc::Id loc; 
    const unsigned char hops_away; // 
}; // should be able to send

}


#endif // HIVE_MAP_CPP_LOCATION_MESSAGES_H_
