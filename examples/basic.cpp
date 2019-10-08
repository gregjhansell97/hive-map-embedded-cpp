#include <stdlib.h>
#include <unistd.h>

#include "hive_map.hpp"


#include <iostream>

#define SENSOR_SPACE_ID 10
#define LOCATION 10
#define GOAL_LOCATION 1


using std::cout;
using std::endl;

class SerialChannel: public hmap::Channel {
public:
    void write(char* data, size_t len) { }
    void read(char** data, size_t len) { }
};

struct SensorSpace {
    static hmap::SpaceId id = SENSOR_SPACE_ID;
    unsigned char sensor;
};

typedef hmap::Node<struct SensorSpace> SensorNode;

int main() {
    SensorNode node(LOCATION, GOAL_LOCATION); // location and goal location
    SerialChannel serial_1;
    hmap::Channel* channels = {&serial_1};
    node.set_channels(channels, 1);

    cout << NODE_LOCATION_ID << endl;
    while(1) {
        usleep(1000);
        node.cycle();
        node.state.sensor = 11;
        // update if state changed
        node.update();
    }


    exit(EXIT_SUCCESS);
}
