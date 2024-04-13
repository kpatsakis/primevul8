search_base_identity(struct lys_ident *der, struct lys_ident *base)
{
    int i;

    if (der == base) {
        return 1;
    } else {
        for(i = 0; i < der->base_size; i++) {
            if (search_base_identity(der->base[i], base) == 1) {
                return 1;
            }
        }
    }

    return 0;
}