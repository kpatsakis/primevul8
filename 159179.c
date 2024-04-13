oftrace_report(struct ovs_list *super, enum oftrace_node_type type,
               const char *text)
{
    struct oftrace_node *node = xmalloc(sizeof *node);
    ovs_list_push_back(super, &node->node);
    node->type = type;
    node->text = xstrdup(text);
    ovs_list_init(&node->subs);

    return node;
}