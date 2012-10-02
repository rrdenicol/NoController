#ifndef __OFP_H__
#define __OFP_H__

/* Basic OpenFlow include. This is assumed to be valid for OpenFlow versions. */

#include <stdint.h>

enum ofp_type {
    OFPT_HELLO = 0x0,
    OFPT_ERROR = 0x1,
    OFPT_ECHO_REQUEST = 0x2,
    OFPT_ECHO_REPLY = 0x3,
    OFPT_FEATURES_REQUEST = 0x5,
    OFPT_FEATURES_REPLY = 0x6,
};

struct ofp_header {
    uint8_t version;
    uint8_t type;
    uint16_t length;
    uint32_t xid;
};

struct ofp_switch_features {
    struct ofp_header header;
    uint64_t datapath_id;
};

// Retrieve basic OpenFlow header fields
uint8_t ofp_header_get_version(void* ofp_msg);
uint8_t ofp_header_get_type(void* ofp_msg);
uint16_t ofp_header_get_length(void* ofp_msg);
void ofp_header_set_length(void* ofp_msg, uint16_t length);
uint16_t ofp_header_get_xid(void* ofp_msg);
uint64_t ofp_switch_features_get_datapath_id(void* ofp_msg);

#endif /* __OFP_H__ */


