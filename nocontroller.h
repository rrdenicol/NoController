#ifndef __NOCONTROLLER_H__
#define __NOCONTROLLER_H__

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "utils.h"

#define PORT 6633
#define MAX_CONNS 1024
#define DEFAULT_OFP_VERSION 0x01

typedef struct config {
    uint8_t VERSION;
    bool DEAL_WITH_HELLO;
    bool DEAL_WITH_ECHO_REQUEST;
    bool DEAL_WITH_SWITCH_FEATURES;
    bool DEAL_WITH_SENDING_ECHO_REQUESTS;
} CONFIG;

void configure(struct config appconfig);

pthread_t add_message_sender(CONNECTION* conn, void *(*function) (void *));

// User application functions
void init(CONNECTION* conn);
MESSAGE* process_message(CONNECTION* connection, MESSAGE* msg);

#endif /* __NOCONTROLLER_H__ */
