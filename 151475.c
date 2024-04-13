check_instid_ext_dep(const struct lys_node *sleaf, const char *json_instid)
{
    const struct lys_node *op_node, *first_node;
    enum int_log_opts prev_ilo;
    char *buf, *tmp;

    if (!json_instid || !json_instid[0]) {
        /* no/empty value */
        return 0;
    }

    for (op_node = lys_parent(sleaf);
         op_node && !(op_node->nodetype & (LYS_NOTIF | LYS_RPC | LYS_ACTION));
         op_node = lys_parent(op_node));

    if (op_node && lys_parent(op_node)) {
        /* nested operation - any absolute path is external */
        return 1;
    }

    /* get the first node from the instid */
    tmp = strchr(json_instid + 1, '/');
    buf = strndup(json_instid, tmp ? (size_t)(tmp - json_instid) : strlen(json_instid));
    if (!buf) {
        /* so that we do not have to bother with logging, say it is not external */
        return 0;
    }

    /* find the first schema node, do not log */
    ly_ilo_change(NULL, ILO_IGNORE, &prev_ilo, NULL);
    first_node = ly_ctx_get_node(NULL, sleaf, buf, 0);
    ly_ilo_restore(NULL, prev_ilo, NULL, 0);

    free(buf);
    if (!first_node) {
        /* unknown path, say it is external */
        return 1;
    }

    /* based on the first schema node in the path we can decide whether it points to an external tree or not */

    if (op_node) {
        if (op_node != first_node) {
            /* it is a top-level operation, so we're good if it points somewhere inside it */
            return 1;
        }
    } else {
        if (lys_node_module(sleaf) != lys_node_module(first_node)) {
            /* modules differ */
            return 1;
        }
    }

    return 0;
}