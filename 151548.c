resolve_base_ident_sub(const struct lys_module *module, struct lys_ident *ident, const char *basename,
                       struct unres_schema *unres, struct lys_ident **ret)
{
    uint32_t i, j;
    struct lys_ident *base = NULL;
    struct ly_ctx *ctx = module->ctx;

    assert(ret);

    /* search module */
    for (i = 0; i < module->ident_size; i++) {
        if (!strcmp(basename, module->ident[i].name)) {

            if (!ident) {
                /* just search for type, so do not modify anything, just return
                 * the base identity pointer */
                *ret = &module->ident[i];
                return EXIT_SUCCESS;
            }

            base = &module->ident[i];
            goto matchfound;
        }
    }

    /* search submodules */
    for (j = 0; j < module->inc_size && module->inc[j].submodule; j++) {
        for (i = 0; i < module->inc[j].submodule->ident_size; i++) {
            if (!strcmp(basename, module->inc[j].submodule->ident[i].name)) {

                if (!ident) {
                    *ret = &module->inc[j].submodule->ident[i];
                    return EXIT_SUCCESS;
                }

                base = &module->inc[j].submodule->ident[i];
                goto matchfound;
            }
        }
    }

matchfound:
    /* we found it somewhere */
    if (base) {
        /* is it already completely resolved? */
        for (i = 0; i < unres->count; i++) {
            if ((unres->item[i] == base) && (unres->type[i] == UNRES_IDENT)) {
                /* identity found, but not yet resolved, so do not return it in *res and try it again later */

                /* simple check for circular reference,
                 * the complete check is done as a side effect of using only completely
                 * resolved identities (previous check of unres content) */
                if (ly_strequal((const char *)unres->str_snode[i], ident->name, 1)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, basename, "base");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Circular reference of \"%s\" identity.", basename);
                    return -1;
                }

                return EXIT_FAILURE;
            }
        }

        /* checks done, store the result */
        *ret = base;
        return EXIT_SUCCESS;
    }

    /* base not found (maybe a forward reference) */
    return EXIT_FAILURE;
}