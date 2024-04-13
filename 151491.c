resolve_absolute_schema_nodeid(const char *nodeid, const struct lys_module *module, int ret_nodetype,
                               const struct lys_node **ret)
{
    const char *name, *mod_name, *id;
    const struct lys_node *sibling, *start_parent;
    int r, nam_len, mod_name_len, is_relative = -1;
    const struct lys_module *abs_start_mod;

    assert(nodeid && module && ret);
    assert(!(ret_nodetype & (LYS_USES | LYS_AUGMENT)) && ((ret_nodetype == LYS_GROUPING) || !(ret_nodetype & LYS_GROUPING)));

    id = nodeid;
    start_parent = NULL;

    if ((r = parse_schema_nodeid(id, &mod_name, &mod_name_len, &name, &nam_len, &is_relative, NULL, NULL, 0)) < 1) {
        return ((id - nodeid) - r) + 1;
    }
    id += r;

    if (is_relative) {
        return -1;
    }

    abs_start_mod = lyp_get_module(module, NULL, 0, mod_name, mod_name_len, 0);
    if (!abs_start_mod) {
        return -1;
    }

    while (1) {
        sibling = NULL;
        while ((sibling = lys_getnext(sibling, start_parent, abs_start_mod, LYS_GETNEXT_WITHCHOICE
                | LYS_GETNEXT_WITHCASE | LYS_GETNEXT_WITHINOUT | LYS_GETNEXT_WITHGROUPING | LYS_GETNEXT_NOSTATECHECK))) {
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