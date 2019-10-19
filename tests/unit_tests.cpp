#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "hive_map.hpp"

#define ROOM_1 23
#define OCCUPANCY_DATABASE 100
#define OCCUPANCY_MESSAGE_ID 25

class SerialChannel: public hmap::Channel {
public:
    unsigned long random() { return 5; } 
    void write(char* data, size_t len) override { }
    size_t read(char* data, size_t len) override { }
};

struct OccupancyMsg {
    hmap::msg::Header header {
        .id = OCCUPANCY_MESSAGE_ID,
        .bcast_radius = 1
    };
    hmap::loc::Id room;
    bool occupied;
};


void on_occupancy_msg(void* raw_msg) {
    OccupancyMsg* msg = static_cast<OccupancyMsg*>(msg);
    // handle callback from here
}

int main() {
    hmap::Location room(ROOM_1);
    room.subscribe<OccupancyMsg>(&on_occupancy_msg);

    SerialChannel s;
    room.add_channel(s);

    // we're gonna for-go message sending for now

    hmap::Destination& database = room.destinations(OCCUPANCY_DATABASE);
    while(1) {
        usleep(1000);
        OccupancyMsg m;

        m.room = ROOM_1;
        m.occupied = false;
        database.publish(m);
        

        room.cycle();
    }
    exit(EXIT_SUCCESS);
}
