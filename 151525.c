resolve_iffeature_recursive(struct lys_iffeature *expr, int *index_e, int *index_f)
{
    uint8_t op;
    int a, b;

    op = iff_getop(expr->expr, *index_e);
    (*index_e)++;

    switch (op) {
    case LYS_IFF_F:
        /* resolve feature */
        return resolve_feature_value(expr->features[(*index_f)++]);
    case LYS_IFF_NOT:
        /* invert result */
        return resolve_iffeature_recursive(expr, index_e, index_f) ? 0 : 1;
    case LYS_IFF_AND:
    case LYS_IFF_OR:
        a = resolve_iffeature_recursive(expr, index_e, index_f);
        b = resolve_iffeature_recursive(expr, index_e, index_f);
        if (op == LYS_IFF_AND) {
            return a && b;
        } else { /* LYS_IFF_OR */
            return a || b;
        }
    }

    return 0;
}