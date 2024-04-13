unres_schema_free(struct lys_module *module, struct unres_schema **unres, int all)
{
    uint32_t i;
    unsigned int unresolved = 0;

    if (!unres || !(*unres)) {
        return;
    }

    assert(module || ((*unres)->count == 0));

    for (i = 0; i < (*unres)->count; ++i) {
        if (!all && ((*unres)->module[i] != module)) {
            if ((*unres)->type[i] != UNRES_RESOLVED) {
                unresolved++;
            }
            continue;
        }

        /* free heap memory for the specific item */
        unres_schema_free_item(module->ctx, *unres, i);
    }

    /* free it all */
    if (!module || all || (!unresolved && !module->type)) {
        free((*unres)->item);
        free((*unres)->type);
        free((*unres)->str_snode);
        free((*unres)->module);
        free((*unres));
        (*unres) = NULL;
    }
}