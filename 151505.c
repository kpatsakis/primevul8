resolve_superior_type_check(struct lys_type *type)
{
    uint32_t i;

    if (type->base == LY_TYPE_DER) {
        /* check that the referenced typedef is resolved */
        return EXIT_FAILURE;
    } else if (type->base == LY_TYPE_UNION) {
        /* check that all union types are resolved */
        for (i = 0; i < type->info.uni.count; ++i) {
            if (resolve_superior_type_check(&type->info.uni.types[i])) {
                return EXIT_FAILURE;
            }
        }
    } else if (type->base == LY_TYPE_LEAFREF) {
        /* check there is path in some derived type */
        while (!type->info.lref.path) {
            assert(type->der);
            type = &type->der->type;
        }
    }

    return EXIT_SUCCESS;
}