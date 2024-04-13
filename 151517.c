resolve_uses_schema_nodeid(const char *nodeid, const struct lys_node *start, const struct lys_node_grp **ret)
{
    const struct lys_module *module;
    const char *mod_prefix, *name;
    int i, mod_prefix_len, nam_len;

    /* parse the identifier, it must be parsed on one call */
    if (((i = parse_node_identifier(nodeid, &mod_prefix, &mod_prefix_len, &name, &nam_len, NULL, 0)) < 1) || nodeid[i]) {
        return -i + 1;
    }

    module = lyp_get_module(start->module, mod_prefix, mod_prefix_len, NULL, 0, 0);
    if (!module) {
        return -1;
    }
    if (module != lys_main_module(start->module)) {
        start = module->data;
    }

    *ret = lys_find_grouping_up(name, (struct lys_node *)start);

    return EXIT_SUCCESS;
}