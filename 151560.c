resolve_schema_leafref_valid_dep_flag(const struct lys_node *op_node, const struct lys_module *local_mod,
                                      const struct lys_node *first_node, int abs_path)
{
    int dep1, dep2;
    const struct lys_node *node;

    if (!op_node) {
        /* leafref pointing to a different module */
        if (local_mod != lys_node_module(first_node)) {
            return 1;
        }
    } else if (lys_parent(op_node)) {
        /* inner operation (notif/action) */
        if (abs_path) {
            return 1;
        } else {
            /* compare depth of both nodes */
            for (dep1 = 0, node = op_node; lys_parent(node); node = lys_parent(node));
            for (dep2 = 0, node = first_node; lys_parent(node); node = lys_parent(node));
            if ((dep2 > dep1) || ((dep2 == dep1) && (op_node != first_node))) {
                return 1;
            }
        }
    } else {
        /* top-level operation (notif/rpc) */
        if (op_node != first_node) {
            return 1;
        }
    }

    return 0;
}