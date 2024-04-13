resolve_choice_dflt(struct lys_node_choice *choic, const char *dflt)
{
    struct lys_node *child, *ret;

    LY_TREE_FOR(choic->child, child) {
        if (child->nodetype == LYS_USES) {
            ret = resolve_choice_dflt((struct lys_node_choice *)child, dflt);
            if (ret) {
                return ret;
            }
        }

        if (ly_strequal(child->name, dflt, 1) && (child->nodetype & (LYS_ANYDATA | LYS_CASE
                | LYS_CONTAINER | LYS_LEAF | LYS_LEAFLIST | LYS_LIST | LYS_CHOICE))) {
            return child;
        }
    }

    return NULL;
}