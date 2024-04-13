oftrace_node_destroy(struct oftrace_node *node)
{
    if (node) {
        oftrace_node_list_destroy(&node->subs);
        free(node->text);
        free(node);
    }
}