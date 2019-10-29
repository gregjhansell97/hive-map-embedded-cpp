#include "../hive_map.hpp"
#if 0

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <thread>
#include <iostream>

#include "hive_map.hpp"

#define ROOM_1 11
#define ROOM_2 22
#define ROOM_3 33
#define ROOM_4 44
#define OCCUPANCY_DATABASE 100
#define OCCUPANCY_MESSAGE 25

using std::cout;
using std::endl;

class PipeChannel: public hmap::Channel {
public:
    PipeChannel() {}
    void connect(PipeChannel& p) {
        int pipe_1[2];
        int pipe_2[2];
        pipe(pipe_1);
        pipe(pipe_2);
        m_read_fd = pipe_1[0];
        p.m_write_fd = pipe_1[1];//write
        p.m_read_fd = pipe_2[0];
        m_write_fd = pipe_2[1];//write

        fcntl(m_read_fd, F_SETFL, O_NONBLOCK);
        fcntl(p.m_read_fd, F_SETFL, O_NONBLOCK);
    }
    unsigned char random() override { 
        return (unsigned char)(rand()%255);
    } 
    void send_data(char* data, size_t len) override { 
        write(m_write_fd, data, len);
    }
    size_t recv_data(char* data, size_t len) override { 
        int nread = read(m_read_fd, data, len);
        if(nread == -1) {
            return 0;
        }
        return static_cast<size_t>(nread);
    }
private:
    int m_read_fd;
    int m_write_fd;
};

struct OccupancyMsg {
    hmap::msg::Header header {
        .type = OCCUPANCY_MESSAGE,
        .bcast_radius = 1,
        .destination = hmap::loc::ANY,
        .size = sizeof(OccupancyMsg)
    };
    hmap::loc::Id room;
    bool occupied;
};

void spin(
        const hmap::loc::Id id,
        PipeChannel* channels,
        const size_t len) {
    hmap::Location room(id);
    for(size_t i = 0; i < len; ++i) {
        room.add_channel(channels[i]);
    }

    hmap::Destination& database = room.destinations(OCCUPANCY_DATABASE);
    unsigned int publish_flag = 0;
    OccupancyMsg m;
    m.room = id;
    m.occupied = false;
    while(1) {
        usleep(300000);
        if(publish_flag%10 == 0) {
            m.occupied = !m.occupied; 
            database.publish(m);
        }
        room.cycle();
        ++publish_flag;
    }
}


void on_occupancy_msg(void* raw_msg) {
    OccupancyMsg* msg = static_cast<OccupancyMsg*>(raw_msg);
    cout << (int)msg->room << ": " << msg->occupied << endl;
    // handle callback from here
}

int main() {
    srand(time(NULL));

    PipeChannel room_1_channels[3];
    PipeChannel room_2_channels[2];
    PipeChannel room_3_channels[1];
    PipeChannel room_4_channels[1];
    PipeChannel c;

    room_1_channels[0].connect(c);
    room_1_channels[1].connect(room_2_channels[0]);
    room_1_channels[2].connect(room_4_channels[0]);
    room_2_channels[1].connect(room_3_channels[0]);

    std::thread room_1_thread(spin, ROOM_1, room_1_channels, 3);
    std::thread room_2_thread(spin, ROOM_2, room_2_channels, 2);
    std::thread room_3_thread(spin, ROOM_3, room_3_channels, 1);
    std::thread room_4_thread(spin, ROOM_4, room_4_channels, 1);

    hmap::Location database(OCCUPANCY_DATABASE);
    database.subscribe<OccupancyMsg>(&on_occupancy_msg);
    database.add_channel(c);

    while(1) {
        usleep(300000);
        database.cycle();
    }
    exit(EXIT_SUCCESS);
}

#endif

int main() {

}
