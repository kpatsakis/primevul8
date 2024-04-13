oftrace_node_list_destroy(struct ovs_list *nodes)
{
    if (nodes) {
        struct oftrace_node *node, *next;
        LIST_FOR_EACH_SAFE (node, next, node, nodes) {
            ovs_list_remove(&node->node);
            oftrace_node_destroy(node);
        }
    }
}