#include "nocontroller.h"
#include "openflow/of10.h"

void init(CONNECTION* conn) {
    CONFIG appconfig = {
        .VERSION = OFP_VERSION,
        .DEAL_WITH_HELLO = true,
        .DEAL_WITH_ECHO_REQUEST = true,
        .DEAL_WITH_SWITCH_FEATURES = true,
        .DEAL_WITH_SENDING_ECHO_REQUESTS = true,
    };
    configure(appconfig);
}

MESSAGE* process_message(CONNECTION* conn, MESSAGE* msg) {
    uint8_t type = ofp_header_get_type(msg->data);
    if (type == OFPT_PACKET_IN) {
        MESSAGE* fm = msg_new();
        
        msg_pack_1(fm, "ofp_flow_mod.header.version", 0x01);
        msg_pack_1(fm, "ofp_flow_mod.header.type", OFPT_FLOW_MOD);
        msg_pack_2(fm, "ofp_flow_mod.header.length", 80);
        msg_pack_4(fm, "ofp_flow_mod.header.xid", 0xcafebabe);
        msg_pack_4(fm, "ofp_flow_mod.match.wildcards", -1);
        msg_padding(fm, 36);
        msg_pack_8(fm, "ofp_flow_mod.cookie", 0xdeadbeef);
        msg_pack_2(fm, "ofp_flow_mod.command", OFPFC_ADD);
        msg_pack_2(fm, "ofp_flow_mod.idle_timeout", 60);
        msg_pack_2(fm, "ofp_flow_mod.hard_timeout", 300);
        msg_pack_2(fm, "ofp_flow_mod.priority", 12345);
        msg_pack_4(fm, "ofp_flow_mod.buffer_id", -1);
        msg_pack_2(fm, "ofp_flow_mod.out_port", OFPP_NONE);
        msg_pack_2(fm, "ofp_flow_mod.flags", 0);
        msg_pack_2(fm, "ofp_flow_mod.actions[0].type", OFPAT_OUTPUT);
        msg_pack_2(fm, "ofp_flow_mod.actions[0].len", 8);
        msg_pack_2(fm, "ofp_flow_mod.actions[0].port", OFPP_FLOOD);
        msg_pack_2(fm, "ofp_flow_mod.actions[0].max_len", 256);
        ofp_header_set_length(fm->data, fm->length);
        
        return fm;
    }
    
    return NULL;
}
