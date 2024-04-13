resolve_applies_must(const struct lyd_node *node)
{
    int ret = 0;
    uint8_t must_size;
    struct lys_node *schema, *iter;

    assert(node);

    schema = node->schema;

    /* their own must */
    switch (schema->nodetype) {
    case LYS_CONTAINER:
        must_size = ((struct lys_node_container *)schema)->must_size;
        break;
    case LYS_LEAF:
        must_size = ((struct lys_node_leaf *)schema)->must_size;
        break;
    case LYS_LEAFLIST:
        must_size = ((struct lys_node_leaflist *)schema)->must_size;
        break;
    case LYS_LIST:
        must_size = ((struct lys_node_list *)schema)->must_size;
        break;
    case LYS_ANYXML:
    case LYS_ANYDATA:
        must_size = ((struct lys_node_anydata *)schema)->must_size;
        break;
    case LYS_NOTIF:
        must_size = ((struct lys_node_notif *)schema)->must_size;
        break;
    default:
        must_size = 0;
        break;
    }

    if (must_size) {
        ++ret;
    }

    /* schema may be a direct data child of input/output with must (but it must be first, it needs to be evaluated only once) */
    if (!node->prev->next) {
        for (iter = lys_parent(schema); iter && (iter->nodetype & (LYS_CHOICE | LYS_CASE | LYS_USES)); iter = lys_parent(iter));
        if (iter && (iter->nodetype & (LYS_INPUT | LYS_OUTPUT))) {
            ret += 0x2;
        }
    }

    return ret;
}