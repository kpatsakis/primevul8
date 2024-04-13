unres_schema_free_item(struct ly_ctx *ctx, struct unres_schema *unres, uint32_t i)
{
    struct lyxml_elem *yin;
    struct yang_type *yang;
    struct unres_iffeat_data *iff_data;

    switch (unres->type[i]) {
    case UNRES_TYPE_DER_TPDF:
    case UNRES_TYPE_DER:
        yin = (struct lyxml_elem *)((struct lys_type *)unres->item[i])->der;
        if (yin->flags & LY_YANG_STRUCTURE_FLAG) {
            yang =(struct yang_type *)yin;
            ((struct lys_type *)unres->item[i])->base = yang->base;
            lydict_remove(ctx, yang->name);
            free(yang);
            if (((struct lys_type *)unres->item[i])->base == LY_TYPE_UNION) {
                yang_free_type_union(ctx, (struct lys_type *)unres->item[i]);
            }
        } else {
            lyxml_free(ctx, yin);
        }
        break;
    case UNRES_IFFEAT:
        iff_data = (struct unres_iffeat_data *)unres->str_snode[i];
        lydict_remove(ctx, iff_data->fname);
        free(unres->str_snode[i]);
        break;
    case UNRES_IDENT:
    case UNRES_TYPE_IDENTREF:
    case UNRES_CHOICE_DFLT:
    case UNRES_LIST_KEYS:
        lydict_remove(ctx, (const char *)unres->str_snode[i]);
        break;
    case UNRES_LIST_UNIQ:
        free(unres->item[i]);
        break;
    case UNRES_EXT:
        free(unres->str_snode[i]);
        break;
    case UNRES_EXT_FINALIZE:
        free(unres->str_snode[i]);
    default:
        break;
    }
    unres->type[i] = UNRES_RESOLVED;
}