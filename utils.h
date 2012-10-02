#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <string.h>

typedef struct msg {
    void* data;
    uint32_t length;
} MESSAGE;

typedef struct conn {
    int socket;
    uint64_t dpid;
} CONNECTION;

// Connection
CONNECTION* conn_new(int socket);
int conn_send(CONNECTION* conn, MESSAGE* msg);
int conn_get_socket(CONNECTION* conn);
uint64_t conn_get_dpid(CONNECTION* conn);
void conn_set_dpid(CONNECTION* conn, uint64_t dpid);
void conn_close(CONNECTION* conn);

// Message
MESSAGE* msg_new();

size_t msg_get_length(MESSAGE* msg);
void msg_set_length(MESSAGE* msg, size_t length);

void* msg_get_data(MESSAGE* msg);
void msg_set_data(MESSAGE* msg, void* data);

void msg_pack_1(MESSAGE* msg, char* field, uint8_t value);
void msg_pack_2(MESSAGE* msg, char* field, uint16_t value);
void msg_pack_4(MESSAGE* msg, char* field, uint32_t value);
void msg_pack_8(MESSAGE* msg, char* field, uint64_t value);
void msg_pack(MESSAGE* msg, char* field, void* value, size_t length);

void msg_delete(MESSAGE* msg);

void error(const char* format, ...);
void socketerror(const char *msg);

// Utils
/* hton and ntoh based on lenght */
#define HTON(value, length) hton##length(value)
#define NTOH(value, length) ntoh##length(value)
inline uint8_t hton8(uint8_t n);
inline uint8_t ntoh8(uint8_t n);
inline uint16_t hton16(uint16_t n);
inline uint16_t ntoh16(uint16_t n);
inline uint32_t hton32(uint32_t n);
inline uint32_t ntoh32(uint32_t n);
inline uint64_t hton64(uint64_t n);
inline uint64_t ntoh64(uint64_t n);

#endif /* __UTILS_H__ */


