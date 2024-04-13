unres_schema_add_str(struct lys_module *mod, struct unres_schema *unres, void *item, enum UNRES_ITEM type,
                     const char *str)
{
    int rc;
    const char *dictstr;

    dictstr = lydict_insert(mod->ctx, str, 0);
    rc = unres_schema_add_node(mod, unres, item, type, (struct lys_node *)dictstr);

    if (rc < 0) {
        lydict_remove(mod->ctx, dictstr);
    }
    return rc;
}