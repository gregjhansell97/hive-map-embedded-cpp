#ifndef HIVE_MAP_CPP_HIVE_MAP_HPP_
#define HIVE_MAP_CPP_HIVE_MAP_HPP_

#include <stddef.h>

namespace hmap {

typedef const unsigned char SpaceId;
typedef unsigned int Location;

class Channel {
public:
    virtual void write(char* data, size_t len) = 0;
    virtual void read(char** data, size_t len) = 0;
};

template <typename T>
class Node {
public:
    Node(const Location loc, const Location goal):
        location(loc), 
        goal(goal) { }
    void set_channels(Channel* channels, size_t len) {
        // insert implementation here
    }
    void cycle() {
        // insert implementation here
    }
    void update() {
        // insert implementation here 
    }
    virtual void state_received(
            hmap::Location loc, 
            hmap::SpaceId id, 
            void* raw_state) { }
    T state;
protected:
    const Location location;
    const Location goal;
};

}

#endif // HIVE_MAP_C_HIVE_MAP_H_
