resolve_when_relink_nodes(struct lyd_node *node, struct lyd_node *unlinked_nodes, enum lyxp_node_type ctx_node_type)
{
    struct lyd_node *elem;

    LY_TREE_FOR_SAFE(unlinked_nodes, unlinked_nodes, elem) {
        lyd_unlink_internal(elem, 0);
        if (ctx_node_type == LYXP_NODE_ELEM) {
            if (lyd_insert_common(node, NULL, elem, 0)) {
                return -1;
            }
        } else {
            if (lyd_insert_nextto(node, elem, 0, 0)) {
                return -1;
            }
        }
    }

    return EXIT_SUCCESS;
}