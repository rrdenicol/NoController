#include "ofp.h"
#include "utils.h"

uint8_t ofp_header_get_version(void* ofp_msg) {
    return ((struct ofp_header*) ofp_msg)->version;
}

uint8_t ofp_header_get_type(void* ofp_msg) {
    return ((struct ofp_header*) ofp_msg)->type;
}

void ofp_header_set_length(void* ofp_msg, uint16_t length) {
    ((struct ofp_header*) ofp_msg)->length = HTON(length, 16);
}

uint16_t ofp_header_get_length(void* ofp_msg) {
    return NTOH(((struct ofp_header*) ofp_msg)->length, 16);
}

uint16_t ofp_header_get_xid(void* ofp_msg) {
    return NTOH(((struct ofp_header*) ofp_msg)->xid, 32);
}

uint64_t ofp_switch_features_get_datapath_id(void* ofp_msg) {
    return HTON(((struct ofp_switch_features*) ofp_msg)->datapath_id, 64);
}


