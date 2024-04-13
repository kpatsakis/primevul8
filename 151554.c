resolve_unres_data_autodel_diff(struct unres_data *unres, uint32_t unres_i)
{
    struct lyd_node *next, *child, *parent;
    uint32_t i;

    for (i = 0; i < unres->diff_idx; ++i) {
        if (unres->diff->type[i] == LYD_DIFF_DELETED) {
            /* only leaf(-list) default could be removed and there is nothing to be checked in that case */
            continue;
        }

        if (unres->diff->second[i] == unres->node[unres_i]) {
            /* 1) default value was supposed to be created, but is disabled by when
             * -> remove it from diff altogether
             */
            unres_data_diff_rem(unres, i);
            /* if diff type is CREATED, the value was just a pointer, it can be freed normally (unlike in 4) */
            return;
        } else {
            parent = unres->diff->second[i]->parent;
            while (parent && (parent != unres->node[unres_i])) {
                parent = parent->parent;
            }
            if (parent) {
                /* 2) default value was supposed to be created but is disabled by when in some parent
                 * -> remove this default subtree and add the rest into diff as deleted instead in 4)
                 */
                unres_data_diff_rem(unres, i);
                break;
            }

            LY_TREE_DFS_BEGIN(unres->diff->second[i]->parent, next, child) {
                if (child == unres->node[unres_i]) {
                    /* 3) some default child of a default value was supposed to be created but has false when
                     * -> the subtree will be freed later and automatically disconnected from the diff parent node
                     */
                    return;
                }

                LY_TREE_DFS_END(unres->diff->second[i]->parent, next, child);
            }
        }
    }

    /* 4) it does not overlap with created default values in any way
     * -> just add it into diff as deleted
     */
    unres_data_diff_new(unres, unres->node[unres_i], unres->node[unres_i]->parent, 0);
    lyd_unlink(unres->node[unres_i]);

    /* should not be freed anymore */
    unres->node[unres_i] = NULL;
}