resolve_applies_when(const struct lys_node *schema, int mode, const struct lys_node *stop)
{
    const struct lys_node *parent;

    assert(schema);

    if (!(schema->nodetype & (LYS_NOTIF | LYS_RPC)) && snode_get_when(schema)) {
        return 1;
    }

    parent = schema;
    goto check_augment;

    while (parent) {
        /* stop conditions */
        if (!mode) {
            /* stop on node that can be instantiated in data tree */
            if (!(parent->nodetype & (LYS_USES | LYS_CHOICE | LYS_CASE))) {
                break;
            }
        } else {
            /* stop on the specified node */
            if (parent == stop) {
                break;
            }
        }

        if (snode_get_when(parent)) {
            return 1;
        }
check_augment:

        if (parent->parent && (parent->parent->nodetype == LYS_AUGMENT) && snode_get_when(parent->parent)) {
            return 1;
        }
        parent = lys_parent(parent);
    }

    return 0;
}