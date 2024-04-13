oftrace_recirc_node_destroy(struct oftrace_recirc_node *node)
{
    if (node) {
        recirc_free_id(node->recirc_id);
        dp_packet_delete(node->packet);
        free(node);
    }
}