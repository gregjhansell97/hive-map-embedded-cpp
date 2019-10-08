#include "hive_map.hpp"

#define SENSOR_SPACE_ID 10
#define LOCATION 10
#define GOAL_LOCATION 1

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



SerialChannel serial_1;
SensorNode node(LOCATION, GOAL_LOCATION);

void setup()
{
  node.set_channels({&serial_1}, 1);
  
}

void loop()
{
  node.state.sensor = 11;
  node.update();
}
