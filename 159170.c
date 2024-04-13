ofproto_unixctl_trace(struct unixctl_conn *conn, int argc, const char *argv[],
                      void *aux OVS_UNUSED)
{
    struct ofproto_dpif *ofproto;
    struct dp_packet *packet;
    char *error;
    struct flow flow;
    struct ovs_list next_ct_states = OVS_LIST_INITIALIZER(&next_ct_states);

    error = parse_flow_and_packet(argc, argv, &ofproto, &flow, &packet,
                                  &next_ct_states, NULL);
    if (!error) {
        struct ds result;

        ds_init(&result);
        ofproto_trace(ofproto, &flow, packet, NULL, 0, &next_ct_states,
                      &result);
        unixctl_command_reply(conn, ds_cstr(&result));
        ds_destroy(&result);
        dp_packet_delete(packet);
    } else {
        unixctl_command_reply_error(conn, error);
        free(error);
    }
    free_ct_states(&next_ct_states);
}