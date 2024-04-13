ast_for_exprlist(struct compiling *c, const node *n, expr_context_ty context)
{
    asdl_seq *seq;
    int i;
    expr_ty e;

    REQ(n, exprlist);

    seq = asdl_seq_new((NCH(n) + 1) / 2, c->c_arena);
    if (!seq)
        return NULL;
    for (i = 0; i < NCH(n); i += 2) {
        e = ast_for_expr(c, CHILD(n, i));
        if (!e)
            return NULL;
        asdl_seq_SET(seq, i / 2, e);
        if (context && !set_context(c, e, context, CHILD(n, i)))
            return NULL;
    }
    return seq;
}