#include <stdlib.h>
#include <unistd.h>

#include "hive_map.hpp"

#include "hmap/locations.hpp"
#include "hmap/messages.hpp"

#include <iostream>


using std::cout;
using std::endl;

class SerialChannel: public hmap::Channel {
public:
    void write(char* data, size_t len) { }
    void read(char** data, size_t len) { }
};

int main() {
    Location room(ROOM_1);

    SerialChannel serial_1;
    hmap::Channel* channels = {&serial_1};
    room.set_channels(channels, 1);

    hmap::messages::Occupancy occupancy_msg({occupied: false});

    Destination& database = room.destinations(OCCUPANCY_DATABASE);
    while(1) {
        usleep(1000);
        database.publish(occupancy_msg);
        room.cycle();
    }
    
    exit(EXIT_SUCCESS);
}
