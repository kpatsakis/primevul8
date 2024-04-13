resolve_unres_schema(struct lys_module *mod, struct unres_schema *unres)
{
    uint32_t resolved = 0;

    assert(unres);

    LOGVRB("Resolving \"%s\" unresolved schema nodes and their constraints...", mod->name);

    /* UNRES_TYPE_LEAFREF must be resolved (for storing leafref target pointers);
     * if-features are resolved here to make sure that we will have all if-features for
     * later check of feature circular dependency */
    if (resolve_unres_schema_types(unres, UNRES_USES | UNRES_IFFEAT | UNRES_TYPE_DER | UNRES_TYPE_DER_TPDF | UNRES_TYPE_DER_TPDF
                                   | UNRES_TYPE_LEAFREF | UNRES_MOD_IMPLEMENT | UNRES_AUGMENT | UNRES_CHOICE_DFLT | UNRES_IDENT,
                                   mod->ctx, 1, 0, &resolved)) {
        return -1;
    }

    /* another batch of resolved items */
    if (resolve_unres_schema_types(unres, UNRES_TYPE_IDENTREF | UNRES_FEATURE | UNRES_TYPEDEF_DFLT | UNRES_TYPE_DFLT
                                   | UNRES_LIST_KEYS | UNRES_LIST_UNIQ | UNRES_EXT, mod->ctx, 1, 0, &resolved)) {
        return -1;
    }

    /* print xpath warnings and finalize extensions, keep it last to provide the complete schema tree information to the plugin's checkers */
    if (resolve_unres_schema_types(unres, UNRES_XPATH | UNRES_EXT_FINALIZE, mod->ctx, 0, 1, &resolved)) {
        return -1;
    }

    LOGVRB("All \"%s\" schema nodes and constraints resolved.", mod->name);
    unres->count = 0;
    return EXIT_SUCCESS;
}