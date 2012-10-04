#include "nocontroller.h"
#include "ofp.h"

CONFIG config = {
    .VERSION = DEFAULT_OFP_VERSION,
    .DEAL_WITH_HELLO = true,
    .DEAL_WITH_ECHO_REQUEST = true,
    .DEAL_WITH_SWITCH_FEATURES = true,
    .DEAL_WITH_SENDING_ECHO_REQUESTS = true,
};

MESSAGE* _process_message(CONNECTION* conn, MESSAGE* msg) {
    uint8_t type = ofp_header_get_type(msg->data);
    show_message("Received message type=%d, length=%d at connection=%d\n",
                 type,
                 ofp_header_get_length(msg->data),
                 conn_get_socket(conn));

    if (config.DEAL_WITH_HELLO && type == OFPT_HELLO) {
        if (ofp_header_get_version(msg->data) != config.VERSION) {
            show_message("Incompatible OpenFlow version: app supports %#02x, "\
                         "switch wants %#02x\n",
                         config.VERSION,
                         ofp_header_get_version(msg->data));
            return NULL;
        }

        MESSAGE* reply = msg_new();
        msg_pack_1(reply, "ofp_features_request.header.version", config.VERSION);
        msg_pack_1(reply, "ofp_features_request.header.type", OFPT_FEATURES_REQUEST);
        msg_pack_2(reply, "ofp_features_request.header.length", 0);
        msg_pack_4(reply, "ofp_features_request.header.xid", ofp_header_get_xid(msg->data));
        ofp_header_set_length(reply->data, reply->length);
        return reply;
    }

    if (config.DEAL_WITH_ECHO_REQUEST && type == OFPT_ECHO_REQUEST) {
        MESSAGE* reply = msg_new();
        msg_pack_1(reply, "ofp_echo_reply.header.version", config.VERSION);
        msg_pack_1(reply, "ofp_echo_reply.header.type", OFPT_ECHO_REPLY);
        msg_pack_2(reply, "ofp_echo_reply.header.length", 0);
        msg_pack_4(reply, "ofp_echo_reply.header.xid", ofp_header_get_xid(msg->data));
        ofp_header_set_length(reply->data, reply->length);
        return reply;
    }

    if (config.DEAL_WITH_SWITCH_FEATURES && type == OFPT_FEATURES_REPLY) {
        struct ofp_switch_features* ofp_switch_features = (struct ofp_switch_features*) msg->data;

        uint64_t dpid = ofp_switch_features_get_datapath_id(msg->data);
        show_message("Switch dp_id=%lld\n", dpid);
        conn_set_dpid(conn, dpid);
    }

    if (type == OFPT_ERROR) {
        show_message("Received error message. Redirecting to app.\n");
    }
    
    return process_message(conn, msg);
}

void* _start_openflow_connection(void* arg) {
    CONNECTION* conn = (CONNECTION *) arg;

    MESSAGE* hello = msg_new();
    msg_pack_1(hello, "ofp_hello.header.version", config.VERSION);
    msg_pack_1(hello, "ofp_hello.header.type", OFPT_HELLO);
    msg_pack_2(hello, "ofp_hello.header.length", 0);
    msg_pack_4(hello, "ofp_hello.header.xid", 123);
    ofp_header_set_length(hello->data, hello->length);
    conn_send(conn, hello);
}

void* _keep_echoing(void* arg) {
    CONNECTION* conn = (CONNECTION *) arg;

    while (1) {
        // TODO: define value...
        sleep(5);
        MESSAGE* request = msg_new();
        msg_pack_1(request, "ofp_echo_request.header.version", config.VERSION);
        msg_pack_1(request, "ofp_echo_request.header.type", OFPT_ECHO_REQUEST);
        msg_pack_2(request, "ofp_echo_request.header.length", 0);
        msg_pack_4(request, "ofp_echo_request.header.xid", 123);
        ofp_header_set_length(request->data, request->length);
        conn_send(conn, request);
    }
}

void* _handle_input(void* arg) {
    struct ofp_header header_buffer;
    char* data;

    CONNECTION* conn = (CONNECTION *) arg;

    init(conn);

    if (config.DEAL_WITH_HELLO)
        add_message_sender(conn, &_start_openflow_connection);

    if (config.DEAL_WITH_SENDING_ECHO_REQUESTS)
        add_message_sender(conn, &_keep_echoing);

    struct pollfd pollfd;
    pollfd.fd = conn_get_socket(conn);
    pollfd.events = POLLIN;

    while (poll(&pollfd, 1, -1) >= 0) {
        int n = conn_recv(conn, &header_buffer, sizeof(struct ofp_header));
        if (n == 0)
            break;

        int msglen = ofp_header_get_length(&header_buffer);

        // Read the message
        MESSAGE* msg = msg_new();
        msg_pack(msg, "header", &header_buffer, sizeof(struct ofp_header));
        char* body = (char*) malloc(msglen);
        if (msglen > sizeof(struct ofp_header))
            n = conn_recv(conn, body, msglen - sizeof(struct ofp_header));
        msg_pack(msg, "body", body, sizeof(struct ofp_header));

        // Send the response
        MESSAGE* response = _process_message(conn, msg);
        if (response != NULL) {
            n = conn_send(conn, response);
            if (n < 0)
                socketerror("Error writing to socket");
        }
    }

    conn_close(conn);
    show_message("Connection id=%d, datapath=%lld closed\n", 
                 conn_get_socket(conn), 
                 conn_get_dpid(conn));
}

pthread_t add_message_sender(CONNECTION* conn, void *(*function) (void *)) {
    pthread_t msgrthread;
    pthread_create(&msgrthread, NULL, function, (void *) conn);
    return msgrthread;
}

void configure(struct config appconfig) {
    config.VERSION = appconfig.VERSION;
    config.DEAL_WITH_HELLO = appconfig.DEAL_WITH_HELLO;
    config.DEAL_WITH_ECHO_REQUEST = appconfig.DEAL_WITH_ECHO_REQUEST;
    config.DEAL_WITH_SWITCH_FEATURES = appconfig.DEAL_WITH_SWITCH_FEATURES;
    config.DEAL_WITH_SENDING_ECHO_REQUESTS = appconfig.DEAL_WITH_SENDING_ECHO_REQUESTS;
}

int main(int argc, char *argv[]) {
    int listensocket, clientsocket;
    struct sockaddr_in serv_addr, addr;
    socklen_t addrlen = sizeof(addr);
    int i = 0;

    config.DEAL_WITH_HELLO = true;
    config.DEAL_WITH_ECHO_REQUEST = true;
    config.DEAL_WITH_SWITCH_FEATURES = true;
    config.DEAL_WITH_SENDING_ECHO_REQUESTS = true;

    listensocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listensocket < 0)
        socketerror("Error opening listen socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(listensocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        socketerror("Error binding");
    listen(listensocket, MAX_CONNS);

    while (1) {
        clientsocket = accept(listensocket, (struct sockaddr *) &addr, &addrlen);
        if (clientsocket < 0)
            socketerror("Error accepting");

        CONNECTION* conn = conn_new(clientsocket);

        pthread_t inputthread;
        pthread_create(&inputthread, NULL, _handle_input, (void *) conn);

        i++;
    }

    return 0;
}
