check_xpath(struct lys_node *node, int check_place)
{
    struct lys_node *parent;
    struct lyxp_set set;
    enum int_log_opts prev_ilo;

    if (check_place) {
        parent = node;
        while (parent) {
            if (parent->nodetype == LYS_GROUPING) {
                /* unresolved grouping, skip for now (will be checked later) */
                return EXIT_SUCCESS;
            }
            if (parent->nodetype == LYS_AUGMENT) {
                if (!((struct lys_node_augment *)parent)->target) {
                    /* unresolved augment, skip for now (will be checked later) */
                    return EXIT_FAILURE;
                } else {
                    parent = ((struct lys_node_augment *)parent)->target;
                    continue;
                }
            }
            parent = parent->parent;
        }
    }

    memset(&set, 0, sizeof set);

    /* produce just warnings */
    ly_ilo_change(NULL, ILO_ERR2WRN, &prev_ilo, NULL);
    lyxp_node_atomize(node, &set, 1);
    ly_ilo_restore(NULL, prev_ilo, NULL, 0);

    if (set.val.snodes) {
        free(set.val.snodes);
    }
    return EXIT_SUCCESS;
}