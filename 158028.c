ast_for_except_clause(struct compiling *c, const node *exc, node *body)
{
    /* except_clause: 'except' [test ['as' test]] */
    int end_lineno, end_col_offset;
    REQ(exc, except_clause);
    REQ(body, suite);

    if (NCH(exc) == 1) {
        asdl_seq *suite_seq = ast_for_suite(c, body);
        if (!suite_seq)
            return NULL;
        get_last_end_pos(suite_seq, &end_lineno, &end_col_offset);

        return ExceptHandler(NULL, NULL, suite_seq, LINENO(exc),
                             exc->n_col_offset,
                             end_lineno, end_col_offset, c->c_arena);
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
        get_last_end_pos(suite_seq, &end_lineno, &end_col_offset);

        return ExceptHandler(expression, NULL, suite_seq, LINENO(exc),
                             exc->n_col_offset,
                             end_lineno, end_col_offset, c->c_arena);
    }
    else if (NCH(exc) == 4) {
        asdl_seq *suite_seq;
        expr_ty expression;
        identifier e = NEW_IDENTIFIER(CHILD(exc, 3));
        if (!e)
            return NULL;
        if (forbidden_name(c, e, CHILD(exc, 3), 0))
            return NULL;
        expression = ast_for_expr(c, CHILD(exc, 1));
        if (!expression)
            return NULL;
        suite_seq = ast_for_suite(c, body);
        if (!suite_seq)
            return NULL;
        get_last_end_pos(suite_seq, &end_lineno, &end_col_offset);

        return ExceptHandler(expression, e, suite_seq, LINENO(exc),
                             exc->n_col_offset,
                             end_lineno, end_col_offset, c->c_arena);
    }

    PyErr_Format(PyExc_SystemError,
                 "wrong number of children for 'except' clause: %d",
                 NCH(exc));
    return NULL;
}