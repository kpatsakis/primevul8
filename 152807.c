ast_for_print_stmt(struct compiling *c, const node *n)
{
    /* print_stmt: 'print' ( [ test (',' test)* [','] ]
                             | '>>' test [ (',' test)+ [','] ] )
     */
    expr_ty dest = NULL, expression;
    asdl_seq *seq = NULL;
    bool nl;
    int i, j, values_count, start = 1;

    REQ(n, print_stmt);
    if (NCH(n) >= 2 && TYPE(CHILD(n, 1)) == RIGHTSHIFT) {
        dest = ast_for_expr(c, CHILD(n, 2));
        if (!dest)
            return NULL;
            start = 4;
    }
    values_count = (NCH(n) + 1 - start) / 2;
    if (values_count) {
        seq = asdl_seq_new(values_count, c->c_arena);
        if (!seq)
            return NULL;
        for (i = start, j = 0; i < NCH(n); i += 2, ++j) {
            expression = ast_for_expr(c, CHILD(n, i));
            if (!expression)
                return NULL;
            asdl_seq_SET(seq, j, expression);
        }
    }
    nl = (TYPE(CHILD(n, NCH(n) - 1)) == COMMA) ? false : true;
    return Print(dest, seq, nl, LINENO(n), n->n_col_offset, c->c_arena);
}