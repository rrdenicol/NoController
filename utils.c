#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"

/* CONNECTION */
CONNECTION* conn_new(int socket) {
    CONNECTION* conn = (CONNECTION*) malloc(sizeof(CONNECTION));
    conn->socket = socket;
    conn->dpid = 0;
    return conn;
}

int conn_recv(CONNECTION* conn, void* dst, size_t length) {
    return read(conn->socket, (char*) dst, length);
}

int conn_send(CONNECTION* conn, MESSAGE* msg) {
    return write(conn->socket, msg->data, msg->length);
}

int conn_get_socket(CONNECTION* conn) {
    return conn->socket;
}

uint64_t conn_get_dpid(CONNECTION* conn) {
    return conn->dpid;
}

void conn_set_dpid(CONNECTION* conn, uint64_t dpid) {
    conn->dpid = dpid;
}

void conn_close(CONNECTION* conn) {
    close(conn->socket);
}

/* MESSAGE */
MESSAGE* msg_new() {
    MESSAGE* msg = (MESSAGE*) malloc(sizeof(MESSAGE));
    msg->data = NULL;
    msg->length = 0;
    return msg;
}

/* uint*_t type based on length */
#define TYPE(length) uint##length##_t
/* Packs a value of the given length to a message */
#define PACK(msg, name, value, vlength) \
    msg->data = realloc(msg->data, msg->length + vlength/8); \
    *((TYPE(vlength)*) (msg->data + msg->length)) = HTON(value, vlength); \
    msg->length += vlength/8
    
void msg_pack_1(MESSAGE* msg, char* field, uint8_t value) {
    PACK(msg, field, value, 8);
}

void msg_pack_2(MESSAGE* msg, char* field, uint16_t value) {
    PACK(msg, field, value, 16);
}

void msg_pack_4(MESSAGE* msg, char* field, uint32_t value) {
    PACK(msg, field, value, 32);
}

void msg_pack_8(MESSAGE* msg, char* field, uint64_t value) {
    PACK(msg, field, value, 64);
}

void msg_pack(MESSAGE* msg, char* field, void* value, size_t length) {
    msg->data = realloc(msg->data, msg->length + length);
    memcpy(msg->data + msg->length, value, length);
    msg->length += length;
}

void msg_padding(MESSAGE* msg, size_t length) {
    void* padding = (void*) calloc(1, length);
    msg_pack(msg, "padding", padding, length);
    free(padding);
}

void msg_delete(MESSAGE* msg) {
    free(msg->data);
    free(msg);
}

/* Convenience functions */
void show_message(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void socketerror(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

inline uint8_t hton8(uint8_t n) {
    return n;
}

inline uint8_t ntoh8(uint8_t n) {
    return n;
}

inline uint16_t hton16(uint16_t n) {
    return htons(n);
}

inline uint16_t ntoh16(uint16_t n) {
    return ntohs(n);
}

inline uint32_t hton32(uint32_t n) {
    return htonl(n);
}

inline uint32_t ntoh32(uint32_t n) {
    return ntohl(n);
}

inline uint64_t hton64(uint64_t n) {
#if __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
    return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
#endif
}

inline uint64_t ntoh64(uint64_t n) {
#if __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
    return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
#endif
}
