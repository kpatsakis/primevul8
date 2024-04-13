ast_for_itercomp(struct compiling *c, const node *n, int type)
{
    expr_ty elt;
    asdl_seq *comps;

    assert(NCH(n) > 1);

    elt = ast_for_expr(c, CHILD(n, 0));
    if (!elt)
        return NULL;

    comps = ast_for_comprehension(c, CHILD(n, 1));
    if (!comps)
        return NULL;

    if (type == COMP_GENEXP)
        return GeneratorExp(elt, comps, LINENO(n), n->n_col_offset, c->c_arena);
    else if (type == COMP_SETCOMP)
        return SetComp(elt, comps, LINENO(n), n->n_col_offset, c->c_arena);
    else
        /* Should never happen */
        return NULL;
}