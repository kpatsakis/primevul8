resolve_base_ident(const struct lys_module *module, struct lys_ident *ident, const char *basename, const char *parent,
                   struct lys_type *type, struct unres_schema *unres)
{
    const char *name;
    int mod_name_len = 0, rc;
    struct lys_ident *target, **ret;
    uint16_t flags;
    struct lys_module *mod;
    struct ly_ctx *ctx = module->ctx;

    assert((ident && !type) || (!ident && type));

    if (!type) {
        /* have ident to resolve */
        ret = &target;
        flags = ident->flags;
        mod = ident->module;
    } else {
        /* have type to fill */
        ++type->info.ident.count;
        type->info.ident.ref = ly_realloc(type->info.ident.ref, type->info.ident.count * sizeof *type->info.ident.ref);
        LY_CHECK_ERR_RETURN(!type->info.ident.ref, LOGMEM(ctx), -1);

        ret = &type->info.ident.ref[type->info.ident.count - 1];
        flags = type->parent->flags;
        mod = type->parent->module;
    }
    *ret = NULL;

    /* search for the base identity */
    name = strchr(basename, ':');
    if (name) {
        /* set name to correct position after colon */
        mod_name_len = name - basename;
        name++;

        if (!strncmp(basename, module->name, mod_name_len) && !module->name[mod_name_len]) {
            /* prefix refers to the current module, ignore it */
            mod_name_len = 0;
        }
    } else {
        name = basename;
    }

    /* get module where to search */
    module = lyp_get_module(module, NULL, 0, mod_name_len ? basename : NULL, mod_name_len, 0);
    if (!module) {
        /* identity refers unknown data model */
        LOGVAL(ctx, LYE_INMOD, LY_VLOG_NONE, NULL, basename);
        return -1;
    }

    /* search in the identified module ... */
    rc = resolve_base_ident_sub(module, ident, name, unres, ret);
    if (!rc) {
        assert(*ret);

        /* check status */
        if (lyp_check_status(flags, mod, ident ? ident->name : "of type",
                             (*ret)->flags, (*ret)->module, (*ret)->name, NULL)) {
            rc = -1;
        } else if (ident) {
            ident->base[ident->base_size++] = *ret;
            if (lys_main_module(mod)->implemented) {
                /* in case of the implemented identity, maintain backlinks to it
                 * from the base identities to make it available when resolving
                 * data with the identity values (not implemented identity is not
                 * allowed as an identityref value). */
                resolve_identity_backlink_update(ident, *ret);
            }
        }
    } else if (rc == EXIT_FAILURE) {
        LOGVAL(ctx, LYE_INRESOLV, LY_VLOG_NONE, NULL, parent, basename);
        if (type) {
            --type->info.ident.count;
        }
    }

    return rc;
}