ast_for_power(struct compiling *c, const node *n)
{
    /* power: atom trailer* ('**' factor)*
     */
    int i;
    expr_ty e, tmp;
    REQ(n, power);
    e = ast_for_atom(c, CHILD(n, 0));
    if (!e)
        return NULL;
    if (NCH(n) == 1)
        return e;
    for (i = 1; i < NCH(n); i++) {
        node *ch = CHILD(n, i);
        if (TYPE(ch) != trailer)
            break;
        tmp = ast_for_trailer(c, ch, e);
        if (!tmp)
            return NULL;
        tmp->lineno = e->lineno;
        tmp->col_offset = e->col_offset;
        e = tmp;
    }
    if (TYPE(CHILD(n, NCH(n) - 1)) == factor) {
        expr_ty f = ast_for_expr(c, CHILD(n, NCH(n) - 1));
        if (!f)
            return NULL;
        tmp = BinOp(e, Pow, f, LINENO(n), n->n_col_offset, c->c_arena);
        if (!tmp)
            return NULL;
        e = tmp;
    }
    return e;
}