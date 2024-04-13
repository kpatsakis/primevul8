resolve_identity_backlink_update(struct lys_ident *der, struct lys_ident *base)
{
    int i;

    assert(der && base);

    if (!base->der) {
        /* create a set for backlinks if it does not exist */
        base->der = ly_set_new();
    }
    /* store backlink */
    ly_set_add(base->der, der, LY_SET_OPT_USEASLIST);

    /* do it recursively */
    for (i = 0; i < base->base_size; i++) {
        resolve_identity_backlink_update(der, base->base[i]);
    }
}