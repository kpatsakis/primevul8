resolve_unres_data_item(struct lyd_node *node, enum UNRES_ITEM type, int ignore_fail, struct lys_when **failed_when)
{
    int rc, req_inst, ext_dep;
    struct lyd_node_leaf_list *leaf;
    struct lyd_node *ret;
    struct lys_node_leaf *sleaf;

    leaf = (struct lyd_node_leaf_list *)node;
    sleaf = (struct lys_node_leaf *)leaf->schema;

    switch (type) {
    case UNRES_LEAFREF:
        assert(sleaf->type.base == LY_TYPE_LEAFREF);
        assert(leaf->validity & LYD_VAL_LEAFREF);
        if ((ignore_fail == 1) || ((leaf->schema->flags & LYS_LEAFREF_DEP) && (ignore_fail == 2))) {
            req_inst = -1;
        } else {
            req_inst = sleaf->type.info.lref.req;
        }
        rc = resolve_leafref(leaf, sleaf->type.info.lref.path, req_inst, &ret);
        if (!rc) {
            if (ret && !(leaf->schema->flags & LYS_LEAFREF_DEP)) {
                /* valid resolved */
                if (leaf->value_type == LY_TYPE_BITS) {
                    free(leaf->value.bit);
                }
                leaf->value.leafref = ret;
                leaf->value_type = LY_TYPE_LEAFREF;
                leaf->value_flags &= ~LY_VALUE_UNRES;
            } else {
                /* valid unresolved */
                if (!(leaf->value_flags & LY_VALUE_UNRES)) {
                    if (!lyp_parse_value(&sleaf->type, &leaf->value_str, NULL, leaf, NULL, NULL, 1, 0, 0)) {
                        return -1;
                    }
                }
            }
            leaf->validity &= ~LYD_VAL_LEAFREF;
        } else {
            return rc;
        }
        break;

    case UNRES_INSTID:
        assert(sleaf->type.base == LY_TYPE_INST);
        ext_dep = check_instid_ext_dep(leaf->schema, leaf->value_str);
        if (ext_dep == -1) {
            return -1;
        }

        if ((ignore_fail == 1) || (ext_dep && (ignore_fail == 2))) {
            req_inst = -1;
        } else {
            req_inst = sleaf->type.info.inst.req;
        }
        rc = resolve_instid(node, leaf->value_str, req_inst, &ret);
        if (!rc) {
            if (ret && !ext_dep) {
                /* valid resolved */
                leaf->value.instance = ret;
                leaf->value_type = LY_TYPE_INST;
                leaf->value_flags &= ~LY_VALUE_UNRES;
            } else {
                /* valid unresolved */
                leaf->value.instance = NULL;
                leaf->value_type = LY_TYPE_INST;
                leaf->value_flags |= LY_VALUE_UNRES;
            }
        } else {
            return rc;
        }
        break;

    case UNRES_UNION:
        assert(sleaf->type.base == LY_TYPE_UNION);
        return resolve_union(leaf, &sleaf->type, 1, ignore_fail, NULL);

    case UNRES_WHEN:
        if ((rc = resolve_when(node, ignore_fail, failed_when))) {
            return rc;
        }
        break;

    case UNRES_MUST:
        if ((rc = resolve_must(node, 0, ignore_fail))) {
            return rc;
        }
        break;

    case UNRES_MUST_INOUT:
        if ((rc = resolve_must(node, 1, ignore_fail))) {
            return rc;
        }
        break;

    case UNRES_UNIQ_LEAVES:
        if (lyv_data_unique(node)) {
            return -1;
        }
        break;

    default:
        LOGINT(NULL);
        return -1;
    }

    return EXIT_SUCCESS;
}