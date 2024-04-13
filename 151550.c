resolve_when_ctx_node(struct lyd_node *node, struct lys_node *schema, struct lyd_node **ctx_node,
                      enum lyxp_node_type *ctx_node_type)
{
    struct lyd_node *parent;
    struct lys_node *sparent;
    enum lyxp_node_type node_type;
    uint16_t i, data_depth, schema_depth;

    resolve_when_ctx_snode(schema, &schema, &node_type);

    if (node_type == LYXP_NODE_ELEM) {
        /* standard element context node */
        for (parent = node, data_depth = 0; parent; parent = parent->parent, ++data_depth);
        for (sparent = schema, schema_depth = 0;
                sparent;
                sparent = (sparent->nodetype == LYS_AUGMENT ? ((struct lys_node_augment *)sparent)->target : sparent->parent)) {
            if (sparent->nodetype & (LYS_CONTAINER | LYS_LEAF | LYS_LEAFLIST | LYS_LIST | LYS_ANYDATA | LYS_NOTIF | LYS_RPC)) {
                ++schema_depth;
            }
        }
        if (data_depth < schema_depth) {
            return -1;
        }

        /* find the corresponding data node */
        for (i = 0; i < data_depth - schema_depth; ++i) {
            node = node->parent;
        }
        if (node->schema != schema) {
            return -1;
        }
    } else {
        /* root context node */
        while (node->parent) {
            node = node->parent;
        }
        while (node->prev->next) {
            node = node->prev;
        }
    }

    *ctx_node = node;
    *ctx_node_type = node_type;
    return EXIT_SUCCESS;
}