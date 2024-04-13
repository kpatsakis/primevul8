ast_for_ifexpr(struct compiling *c, const node *n)
{
    /* test: or_test 'if' or_test 'else' test */
    expr_ty expression, body, orelse;

    assert(NCH(n) == 5);
    body = ast_for_expr(c, CHILD(n, 0));
    if (!body)
        return NULL;
    expression = ast_for_expr(c, CHILD(n, 2));
    if (!expression)
        return NULL;
    orelse = ast_for_expr(c, CHILD(n, 4));
    if (!orelse)
        return NULL;
    return IfExp(expression, body, orelse, LINENO(n), n->n_col_offset,
                 n->n_end_lineno, n->n_end_col_offset,
                 c->c_arena);
}