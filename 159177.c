execute_actions_except_outputs(struct dpif *dpif,
                               const struct dp_packet *packet,
                               const struct flow *flow,
                               const struct ofpbuf *actions,
                               enum slow_path_reason slow,
                               struct ds *output)
{
    struct ofpbuf pruned_actions;
    ofpbuf_init(&pruned_actions, 0);
    prune_output_actions(actions, &pruned_actions);

    struct dpif_execute execute = {
        .actions = pruned_actions.data,
        .actions_len = pruned_actions.size,
        .needs_help = (slow & SLOW_ACTION) != 0,
        .flow = flow,
        .packet = dp_packet_clone_with_headroom(packet, 2),
    };
    int error = dpif_execute(dpif, &execute);
    if (error) {
        ds_put_format(output, "\nAction execution failed (%s)\n.",
                      ovs_strerror(error));
    }
    dp_packet_delete(execute.packet);
    ofpbuf_uninit(&pruned_actions);
}