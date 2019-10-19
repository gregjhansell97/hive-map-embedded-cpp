#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <fcntl.h>

#include "hive_map.hpp"

#define ROOM_1 23
#define OCCUPANCY_DATABASE 100
#define OCCUPANCY_MESSAGE_ID 25

class PipesChannel: public hmap::Channel {
public:
    PipesChannel(const int read_fd, const int* write_fds, const size_t len):
            m_read_fd(read_fd),
            m_write_fds(write_fds),
            m_len(len) {
        fcntl(read_fd, F_SETFL, O_NONBLOCK);
    }
    unsigned long random() { return 5; } 
    void write(char* data, size_t len) override { 
        /*
        for(size_t i = 0; i < m_len; ++i) {
            write(m_write_fds[i], m_len);
        }*/
        
    }
    size_t read(char* data, size_t len) override { }
private:
    const int m_read_fd;
    const int* m_write_fds;
    const size_t m_len;
};

struct OccupancyMsg {
    hmap::msg::Header header {
        .type = OCCUPANCY_MESSAGE_ID,
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
    srand(time(NULL)); // random seed

    hmap::Location room(ROOM_1);
    room.subscribe<OccupancyMsg>(&on_occupancy_msg);

    //PipesChannel s;
    //room.add_channel(s);

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
