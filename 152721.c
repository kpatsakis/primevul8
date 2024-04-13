ast_for_except_clause(struct compiling *c, const node *exc, node *body)
{
    /* except_clause: 'except' [test [(',' | 'as') test]] */
    REQ(exc, except_clause);
    REQ(body, suite);

    if (NCH(exc) == 1) {
        asdl_seq *suite_seq = ast_for_suite(c, body);
        if (!suite_seq)
            return NULL;

        return ExceptHandler(NULL, NULL, suite_seq, LINENO(exc),
                             exc->n_col_offset, c->c_arena);
    }
    else if (NCH(exc) == 2) {
        expr_ty expression;
        asdl_seq *suite_seq;

        expression = ast_for_expr(c, CHILD(exc, 1));
        if (!expression)
            return NULL;
        suite_seq = ast_for_suite(c, body);
        if (!suite_seq)
            return NULL;

        return ExceptHandler(expression, NULL, suite_seq, LINENO(exc),
                             exc->n_col_offset, c->c_arena);
    }
    else if (NCH(exc) == 4) {
        asdl_seq *suite_seq;
        expr_ty expression;
        expr_ty e = ast_for_expr(c, CHILD(exc, 3));
        if (!e)
            return NULL;
        if (!set_context(c, e, Store, CHILD(exc, 3)))
            return NULL;
        expression = ast_for_expr(c, CHILD(exc, 1));
        if (!expression)
            return NULL;
        suite_seq = ast_for_suite(c, body);
        if (!suite_seq)
            return NULL;

        return ExceptHandler(expression, e, suite_seq, LINENO(exc),
                             exc->n_col_offset, c->c_arena);
    }

    PyErr_Format(PyExc_SystemError,
                 "wrong number of children for 'except' clause: %d",
                 NCH(exc));
    return NULL;
}