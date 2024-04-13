resolve_descendant_schema_nodeid(const char *nodeid, const struct lys_node *start, int ret_nodetype,
                                 int no_innerlist, const struct lys_node **ret)
{
    const char *name, *mod_name, *id;
    const struct lys_node *sibling, *start_parent;
    int r, nam_len, mod_name_len, is_relative = -1;
    /* resolved import module from the start module, it must match the next node-name-match sibling */
    const struct lys_module *module;

    assert(nodeid && ret);
    assert(!(ret_nodetype & (LYS_USES | LYS_AUGMENT | LYS_GROUPING)));

    if (!start) {
        /* leaf not found */
        return 0;
    }

    id = nodeid;
    module = lys_node_module(start);

    if ((r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, NULL, NULL, 0)) < 1) {
        return ((id - nodeid) - r) + 1;
    }
    id += r;

    if (!is_relative) {
        return -1;
    }

    start_parent = lys_parent(start);
    while ((start_parent->nodetype == LYS_USES) && lys_parent(start_parent)) {
        start_parent = lys_parent(start_parent);
    }

    while (1) {
        sibling = NULL;
        while ((sibling = lys_getnext(sibling, start_parent, module,
                LYS_GETNEXT_WITHCHOICE | LYS_GETNEXT_WITHCASE | LYS_GETNEXT_PARENTUSES | LYS_GETNEXT_NOSTATECHECK))) {
            r = schema_nodeid_siblingcheck(sibling, module, mod_name, mod_name_len, name, nam_len);
            if (r == 0) {
                if (!id[0]) {
                    if (!(sibling->nodetype & ret_nodetype)) {
                        /* wrong node type, too bad */
                        continue;
                    }
                    *ret = sibling;
                    return EXIT_SUCCESS;
                }
                start_parent = sibling;
                break;
            } else if (r == 1) {
                continue;
            } else {
                return -1;
            }
        }

        /* no match */
        if (!sibling) {
            *ret = NULL;
            return EXIT_SUCCESS;
        } else if (no_innerlist && sibling->nodetype == LYS_LIST) {
            *ret = NULL;
            return -2;
        }

        if ((r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, NULL, NULL, 0)) < 1) {
            return ((id - nodeid) - r) + 1;
        }
        id += r;
    }

    /* cannot get here */
    LOGINT(module->ctx);
    return -1;
}