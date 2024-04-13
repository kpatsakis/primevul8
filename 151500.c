resolve_feature_value(const struct lys_feature *feat)
{
    int i;

    for (i = 0; i < feat->iffeature_size; i++) {
        if (!resolve_iffeature(&feat->iffeature[i])) {
            return 0;
        }
    }

    return feat->flags & LYS_FENABLED ? 1 : 0;
}