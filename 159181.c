ofproto_trace__(struct ofproto_dpif *ofproto, const struct flow *flow,
                const struct dp_packet *packet, struct ovs_list *recirc_queue,
                const struct ofpact ofpacts[], size_t ofpacts_len,
                struct ds *output)
{
    struct ofpbuf odp_actions;
    ofpbuf_init(&odp_actions, 0);

    struct xlate_in xin;
    struct flow_wildcards wc;
    struct ovs_list trace = OVS_LIST_INITIALIZER(&trace);
    xlate_in_init(&xin, ofproto,
                  ofproto_dpif_get_tables_version(ofproto), flow,
                  flow->in_port.ofp_port, NULL, ntohs(flow->tcp_flags),
                  packet, &wc, &odp_actions);
    xin.ofpacts = ofpacts;
    xin.ofpacts_len = ofpacts_len;
    xin.trace = &trace;
    xin.recirc_queue = recirc_queue;

    /* Copy initial flow out of xin.flow.  It differs from '*flow' because
     * xlate_in_init() initializes actset_output to OFPP_UNSET. */
    struct flow initial_flow = xin.flow;
    ds_put_cstr(output, "Flow: ");
    flow_format(output, &initial_flow, NULL);
    ds_put_char(output, '\n');

    struct xlate_out xout;
    enum xlate_error error = xlate_actions(&xin, &xout);

    oftrace_node_print_details(output, &trace, 0);

    ds_put_cstr(output, "\nFinal flow: ");
    if (flow_equal(&initial_flow, &xin.flow)) {
        ds_put_cstr(output, "unchanged");
    } else {
        flow_format(output, &xin.flow, NULL);
    }
    ds_put_char(output, '\n');

    ds_put_cstr(output, "Megaflow: ");
    struct match match;
    match_init(&match, flow, &wc);
    match_format(&match, NULL, output, OFP_DEFAULT_PRIORITY);
    ds_put_char(output, '\n');

    ds_put_cstr(output, "Datapath actions: ");
    format_odp_actions(output, odp_actions.data, odp_actions.size, NULL);

    if (error != XLATE_OK) {
        ds_put_format(output,
                      "\nTranslation failed (%s), packet is dropped.\n",
                      xlate_strerror(error));
    } else {
        if (xout.slow) {
            explain_slow_path(xout.slow, output);
        }
        if (packet) {
            execute_actions_except_outputs(ofproto->backer->dpif, packet,
                                           &initial_flow, &odp_actions,
                                           xout.slow, output);
        }
    }


    xlate_out_uninit(&xout);
    ofpbuf_uninit(&odp_actions);
    oftrace_node_list_destroy(&trace);
}