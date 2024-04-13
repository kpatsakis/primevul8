check_leafref_config(struct lys_node_leaf *leaf, struct lys_type *type)
{
    unsigned int i;

    if (type->base == LY_TYPE_LEAFREF) {
        if ((leaf->flags & LYS_CONFIG_W) && type->info.lref.target && type->info.lref.req != -1 &&
                (type->info.lref.target->flags & LYS_CONFIG_R)) {
            LOGVAL(leaf->module->ctx, LYE_SPEC, LY_VLOG_LYS, leaf, "The leafref %s is config but refers to a non-config %s.",
                   strnodetype(leaf->nodetype), strnodetype(type->info.lref.target->nodetype));
            return -1;
        }
        /* we can skip the test in case the leafref is not yet resolved. In that case the test is done in the time
         * of leafref resolving (lys_leaf_add_leafref_target()) */
    } else if (type->base == LY_TYPE_UNION) {
        for (i = 0; i < type->info.uni.count; i++) {
            if (check_leafref_config(leaf, &type->info.uni.types[i])) {
                return -1;
            }
        }
    }
    return 0;
}