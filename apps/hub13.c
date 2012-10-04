#include "nocontroller.h"
#include "openflow/of13.h"

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
    
    /* Install default send to controller flow after features reply */
    if (type == OFPT_FEATURES_REPLY) {
        MESSAGE* fm = msg_new();
        msg_pack_1(fm, "ofp_flow_mod.header.version", OFP_VERSION);
        msg_pack_1(fm, "ofp_flow_mod.header.type", OFPT_FLOW_MOD);
        msg_pack_2(fm, "ofp_flow_mod.header.length", 0);
        msg_pack_4(fm, "ofp_flow_mod.header.xid", 0xcafebabe);
        msg_pack_8(fm, "ofp_flow_mod.cookie", 0xdeadbeef);
        msg_pack_8(fm, "ofp_flow_mod.cookie_mask", 0x00);
        msg_pack_1(fm, "ofp_flow_mod.table_id", 0);
        msg_pack_1(fm, "ofp_flow_mod.command", OFPFC_ADD);
        msg_pack_2(fm, "ofp_flow_mod.idle_timeout", 15);
        msg_pack_2(fm, "ofp_flow_mod.hard_timeout", 30);
        msg_pack_2(fm, "ofp_flow_mod.priority", 0);
        msg_pack_4(fm, "ofp_flow_mod.buffer_id", OFP_NO_BUFFER);
        msg_pack_4(fm, "ofp_flow_mod.out_port", OFPP_ANY);
        msg_pack_4(fm, "ofp_flow_mod.out_group", OFPG_ANY);
        msg_pack_2(fm, "ofp_flow_mod.flags", 0);
        msg_padding(fm, 2);
        msg_pack_2(fm, "ofp_flow_mod.match.type", OFPMT_OXM);
        msg_pack_2(fm, "ofp_flow_mod.match.length", 8);
        msg_padding(fm, 4);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].type", OFPIT_APPLY_ACTIONS);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].len", 24);
        msg_padding(fm, 4);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].actions[0].type", OFPAT_OUTPUT);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].actions[0].len", 16);
        msg_pack_4(fm, "ofp_flow_mod.instructions[0].actions[0].port", OFPP_CONTROLLER);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].actions[0].max_len", 256);
        msg_padding(fm, 6);
        ofp_header_set_length(fm->data, fm->length);
        return fm;
    }
        
    else if (type == OFPT_PACKET_IN) {
        MESSAGE* fm = msg_new();
        msg_pack_1(fm, "ofp_flow_mod.header.version", OFP_VERSION);
        msg_pack_1(fm, "ofp_flow_mod.header.type", OFPT_FLOW_MOD);
        msg_pack_2(fm, "ofp_flow_mod.header.length", 0);
        msg_pack_4(fm, "ofp_flow_mod.header.xid", 0xcafebabe);
        msg_pack_8(fm, "ofp_flow_mod.cookie", 0xdeadbeef);
        msg_pack_8(fm, "ofp_flow_mod.cookie_mask", 0x00);
        msg_pack_1(fm, "ofp_flow_mod.table_id", 0);
        msg_pack_1(fm, "ofp_flow_mod.command", OFPFC_ADD);
        msg_pack_2(fm, "ofp_flow_mod.idle_timeout", 15);
        msg_pack_2(fm, "ofp_flow_mod.hard_timeout", 30);
        msg_pack_2(fm, "ofp_flow_mod.priority", 12345);
        msg_pack_4(fm, "ofp_flow_mod.buffer_id", OFP_NO_BUFFER);
        msg_pack_4(fm, "ofp_flow_mod.out_port", OFPP_ANY);
        msg_pack_4(fm, "ofp_flow_mod.out_group", OFPG_ANY);
        msg_pack_2(fm, "ofp_flow_mod.flags", 0);
        msg_padding(fm, 2);
        msg_pack_2(fm, "ofp_flow_mod.match.type", OFPMT_OXM);
        msg_pack_2(fm, "ofp_flow_mod.match.length", 8);
        msg_padding(fm, 4);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].type", OFPIT_APPLY_ACTIONS);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].len", 24);
        msg_padding(fm, 4);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].actions[0].type", OFPAT_OUTPUT);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].actions[0].len", 16);
        msg_pack_4(fm, "ofp_flow_mod.instructions[0].actions[0].port", OFPP_FLOOD);
        msg_pack_2(fm, "ofp_flow_mod.instructions[0].actions[0].max_len", 256);
        msg_padding(fm, 6);
        ofp_header_set_length(fm->data, fm->length);
        return fm;
    }
    
    return NULL;
}
