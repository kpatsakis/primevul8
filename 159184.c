oftrace_add_recirc_node(struct ovs_list *recirc_queue,
                        enum oftrace_recirc_type type, const struct flow *flow,
                        const struct dp_packet *packet, uint32_t recirc_id,
                        const uint16_t zone)
{
    if (!recirc_id_node_find_and_ref(recirc_id)) {
        return false;
    }

    struct oftrace_recirc_node *node = xmalloc(sizeof *node);
    ovs_list_push_back(recirc_queue, &node->node);

    node->type = type;
    node->recirc_id = recirc_id;
    node->flow = *flow;
    node->flow.recirc_id = recirc_id;
    node->flow.ct_zone = zone;
    node->packet = packet ? dp_packet_clone(packet) : NULL;

    return true;
}