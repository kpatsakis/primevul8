snode_get_when(const struct lys_node *schema)
{
    switch (schema->nodetype) {
    case LYS_CONTAINER:
        return ((struct lys_node_container *)schema)->when;
    case LYS_CHOICE:
        return ((struct lys_node_choice *)schema)->when;
    case LYS_LEAF:
        return ((struct lys_node_leaf *)schema)->when;
    case LYS_LEAFLIST:
        return ((struct lys_node_leaflist *)schema)->when;
    case LYS_LIST:
        return ((struct lys_node_list *)schema)->when;
    case LYS_ANYDATA:
    case LYS_ANYXML:
        return ((struct lys_node_anydata *)schema)->when;
    case LYS_CASE:
        return ((struct lys_node_case *)schema)->when;
    case LYS_USES:
        return ((struct lys_node_uses *)schema)->when;
    case LYS_AUGMENT:
        return ((struct lys_node_augment *)schema)->when;
    default:
        return NULL;
    }
}