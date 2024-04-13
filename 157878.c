ast_for_setdisplay(struct compiling *c, const node *n)
{
    int i;
    int size;
    asdl_seq *elts;

    assert(TYPE(n) == (dictorsetmaker));
    size = (NCH(n) + 1) / 2; /* +1 in case no trailing comma */
    elts = _Py_asdl_seq_new(size, c->c_arena);
    if (!elts)
        return NULL;
    for (i = 0; i < NCH(n); i += 2) {
        expr_ty expression;
        expression = ast_for_expr(c, CHILD(n, i));
        if (!expression)
            return NULL;
        asdl_seq_SET(elts, i / 2, expression);
    }
    return Set(elts, LINENO(n), n->n_col_offset,
               n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}