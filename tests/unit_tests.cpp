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

#define ROOM_1 23
#define ROOM_2 32
#define OCCUPANCY_DATABASE 100
#define OCCUPANCY_MESSAGE 25

using std::cout;
using std::endl;

class PipesChannel: public hmap::Channel {
public:
    PipesChannel(int read_fd, int* write_fds, size_t len):
            m_read_fd(read_fd),
            m_write_fds(write_fds),
            m_len(len) {
        fcntl(read_fd, F_SETFL, O_NONBLOCK);
    }
    unsigned long random() { return 5; } 
    void send_data(char* data, size_t len) override { 
        for(size_t i = 0; i < m_len; ++i) {
            write(m_write_fds[i], data, len);
        }
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
    int* m_write_fds;
    size_t m_len;
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
        int read_fd, 
        int* write_fds, 
        const size_t len) {
    PipesChannel channel(read_fd, write_fds, len);
    hmap::Location room(id);
    room.add_channel(channel);

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
    }
}


void on_occupancy_msg(void* raw_msg) {
    OccupancyMsg* msg = static_cast<OccupancyMsg*>(raw_msg);
    cout << msg->occupied << endl;
    // handle callback from here
}

int main() {
    int fd_room_1[2];
    int fd_room_2[2];
    int fd_database[2];
    pipe(fd_room_1);
    pipe(fd_room_2);
    pipe(fd_database);

    int database_write_fds[1];
    database_write_fds[0] = fd_room_1[1];
    PipesChannel channel(fd_database[0], database_write_fds, 1);

    int room_1_write_fds[2];
    room_1_write_fds[0] = fd_database[1];
    room_1_write_fds[1] = fd_room_2[1];
    std::thread room_1_thread(spin, ROOM_1, fd_room_1[0], room_1_write_fds, 2);

    int room_2_write_fds[1];
    room_2_write_fds[0] = fd_room_1[1];
    std::thread room_2_thread(spin, ROOM_2, fd_room_2[0], room_2_write_fds, 1);

    //srand(time(NULL)); // random seed
    // open all pipes
    //
    //
    // spin up location nodes
    

    hmap::Location database(OCCUPANCY_DATABASE);
    database.subscribe<OccupancyMsg>(&on_occupancy_msg);
    database.add_channel(channel);

    while(1) {
        usleep(300000);
        database.cycle();
    }
    exit(EXIT_SUCCESS);
}
