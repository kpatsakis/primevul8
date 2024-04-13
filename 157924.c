ast_for_itercomp(struct compiling *c, const node *n, int type)
{
    /* testlist_comp: (test|star_expr)
     *                ( comp_for | (',' (test|star_expr))* [','] ) */
    expr_ty elt;
    asdl_seq *comps;
    node *ch;

    assert(NCH(n) > 1);

    ch = CHILD(n, 0);
    elt = ast_for_expr(c, ch);
    if (!elt)
        return NULL;
    if (elt->kind == Starred_kind) {
        ast_error(c, ch, "iterable unpacking cannot be used in comprehension");
        return NULL;
    }

    comps = ast_for_comprehension(c, CHILD(n, 1));
    if (!comps)
        return NULL;

    if (type == COMP_GENEXP)
        return GeneratorExp(elt, comps, LINENO(n), n->n_col_offset,
                            n->n_end_lineno, n->n_end_col_offset, c->c_arena);
    else if (type == COMP_LISTCOMP)
        return ListComp(elt, comps, LINENO(n), n->n_col_offset,
                        n->n_end_lineno, n->n_end_col_offset, c->c_arena);
    else if (type == COMP_SETCOMP)
        return SetComp(elt, comps, LINENO(n), n->n_col_offset,
                       n->n_end_lineno, n->n_end_col_offset, c->c_arena);
    else
        /* Should never happen */
        return NULL;
}