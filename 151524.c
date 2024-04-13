unres_data_add(struct unres_data *unres, struct lyd_node *node, enum UNRES_ITEM type)
{
    assert(unres && node);
    assert((type == UNRES_LEAFREF) || (type == UNRES_INSTID) || (type == UNRES_WHEN) || (type == UNRES_MUST)
           || (type == UNRES_MUST_INOUT) || (type == UNRES_UNION) || (type == UNRES_UNIQ_LEAVES));

    unres->count++;
    unres->node = ly_realloc(unres->node, unres->count * sizeof *unres->node);
    LY_CHECK_ERR_RETURN(!unres->node, LOGMEM(NULL), -1);
    unres->node[unres->count - 1] = node;
    unres->type = ly_realloc(unres->type, unres->count * sizeof *unres->type);
    LY_CHECK_ERR_RETURN(!unres->type, LOGMEM(NULL), -1);
    unres->type[unres->count - 1] = type;

    return 0;
}