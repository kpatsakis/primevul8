unres_schema_dup(struct lys_module *mod, struct unres_schema *unres, void *item, enum UNRES_ITEM type, void *new_item)
{
    int i;
    struct unres_list_uniq aux_uniq;
    struct unres_iffeat_data *iff_data;

    assert(item && new_item && ((type != UNRES_LEAFREF) && (type != UNRES_INSTID) && (type != UNRES_WHEN)));

    /* hack for UNRES_LIST_UNIQ, which stores multiple items behind its item */
    if (type == UNRES_LIST_UNIQ) {
        aux_uniq.list = item;
        aux_uniq.expr = ((struct unres_list_uniq *)new_item)->expr;
        item = &aux_uniq;
    }
    i = unres_schema_find(unres, -1, item, type);

    if (i == -1) {
        if (type == UNRES_LIST_UNIQ) {
            free(new_item);
        }
        return EXIT_FAILURE;
    }

    if ((type == UNRES_TYPE_LEAFREF) || (type == UNRES_USES) || (type == UNRES_TYPE_DFLT) ||
            (type == UNRES_FEATURE) || (type == UNRES_LIST_UNIQ)) {
        if (unres_schema_add_node(mod, unres, new_item, type, unres->str_snode[i]) == -1) {
            LOGINT(mod->ctx);
            return -1;
        }
    } else if (type == UNRES_IFFEAT) {
        /* duplicate unres_iffeature_data */
        iff_data = malloc(sizeof *iff_data);
        LY_CHECK_ERR_RETURN(!iff_data, LOGMEM(mod->ctx), -1);
        iff_data->fname = lydict_insert(mod->ctx, ((struct unres_iffeat_data *)unres->str_snode[i])->fname, 0);
        iff_data->node = ((struct unres_iffeat_data *)unres->str_snode[i])->node;
        if (unres_schema_add_node(mod, unres, new_item, type, (struct lys_node *)iff_data) == -1) {
            LOGINT(mod->ctx);
            return -1;
        }
    } else {
        if (unres_schema_add_str(mod, unres, new_item, type, unres->str_snode[i]) == -1) {
            LOGINT(mod->ctx);
            return -1;
        }
    }

    return EXIT_SUCCESS;
}