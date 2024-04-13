ofproto_trace(struct ofproto_dpif *ofproto, const struct flow *flow,
              const struct dp_packet *packet,
              const struct ofpact ofpacts[], size_t ofpacts_len,
              struct ovs_list *next_ct_states, struct ds *output)
{
    struct ovs_list recirc_queue = OVS_LIST_INITIALIZER(&recirc_queue);
    ofproto_trace__(ofproto, flow, packet, &recirc_queue,
                    ofpacts, ofpacts_len, output);

    struct oftrace_recirc_node *recirc_node;
    LIST_FOR_EACH_POP (recirc_node, node, &recirc_queue) {
        ds_put_cstr(output, "\n\n");
        ds_put_char_multiple(output, '=', 79);
        ds_put_format(output, "\nrecirc(%#"PRIx32")",
                      recirc_node->recirc_id);

        if (recirc_node->type == OFT_RECIRC_CONNTRACK) {
            uint32_t ct_state;
            if (ovs_list_is_empty(next_ct_states)) {
                ct_state = CS_TRACKED | CS_NEW;
                ds_put_cstr(output, " - resume conntrack with default "
                            "ct_state=trk|new (use --ct-next to customize)");
            } else {
                ct_state = oftrace_pop_ct_state(next_ct_states);
                struct ds s = DS_EMPTY_INITIALIZER;
                format_flags(&s, ct_state_to_string, ct_state, '|');
                ds_put_format(output, " - resume conntrack with ct_state=%s",
                              ds_cstr(&s));
                ds_destroy(&s);
            }
            recirc_node->flow.ct_state = ct_state;
        }
        ds_put_char(output, '\n');
        ds_put_char_multiple(output, '=', 79);
        ds_put_cstr(output, "\n\n");

        ofproto_trace__(ofproto, &recirc_node->flow, recirc_node->packet,
                        &recirc_queue, ofpacts, ofpacts_len, output);
        oftrace_recirc_node_destroy(recirc_node);
    }
}