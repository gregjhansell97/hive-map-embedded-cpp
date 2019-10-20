#include "channel.h"

namespace hmap {

    bool Channel::is_prior_msg(msg::Hash h) {
        for(size_t i = 0; i < m_msgs_rcvd_len; ++i) {
            if(h == m_msgs_rcvd[MSGS_RCVD_BUFFER]) {
                return true;
            }
        }
        m_msgs_rcvd[front] = h;
        if(m_msgs_rcvd_len < MSGS_RCVD_BUFFER) {
            m_msgs_rcvd_len = m_msgs_rcvd_len + 1;
        }
        front = (front + 1)%m_msgs_rcvd_len;
        return false;
    }

} // hmap
