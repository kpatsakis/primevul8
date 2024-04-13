ast_for_try_stmt(struct compiling *c, const node *n)
{
    const int nch = NCH(n);
    int end_lineno, end_col_offset, n_except = (nch - 3)/3;
    asdl_seq *body, *handlers = NULL, *orelse = NULL, *finally = NULL;
    excepthandler_ty last_handler;

    REQ(n, try_stmt);

    body = ast_for_suite(c, CHILD(n, 2));
    if (body == NULL)
        return NULL;

    if (TYPE(CHILD(n, nch - 3)) == NAME) {
        if (strcmp(STR(CHILD(n, nch - 3)), "finally") == 0) {
            if (nch >= 9 && TYPE(CHILD(n, nch - 6)) == NAME) {
                /* we can assume it's an "else",
                   because nch >= 9 for try-else-finally and
                   it would otherwise have a type of except_clause */
                orelse = ast_for_suite(c, CHILD(n, nch - 4));
                if (orelse == NULL)
                    return NULL;
                n_except--;
            }

            finally = ast_for_suite(c, CHILD(n, nch - 1));
            if (finally == NULL)
                return NULL;
            n_except--;
        }
        else {
            /* we can assume it's an "else",
               otherwise it would have a type of except_clause */
            orelse = ast_for_suite(c, CHILD(n, nch - 1));
            if (orelse == NULL)
                return NULL;
            n_except--;
        }
    }
    else if (TYPE(CHILD(n, nch - 3)) != except_clause) {
        ast_error(c, n, "malformed 'try' statement");
        return NULL;
    }

    if (n_except > 0) {
        int i;
        /* process except statements to create a try ... except */
        handlers = _Py_asdl_seq_new(n_except, c->c_arena);
        if (handlers == NULL)
            return NULL;

        for (i = 0; i < n_except; i++) {
            excepthandler_ty e = ast_for_except_clause(c, CHILD(n, 3 + i * 3),
                                                       CHILD(n, 5 + i * 3));
            if (!e)
                return NULL;
            asdl_seq_SET(handlers, i, e);
        }
    }

    assert(finally != NULL || asdl_seq_LEN(handlers));
        if (finally != NULL) {
        // finally is always last
        get_last_end_pos(finally, &end_lineno, &end_col_offset);
    } else if (orelse != NULL) {
        // otherwise else is last
        get_last_end_pos(orelse, &end_lineno, &end_col_offset);
    } else {
        // inline the get_last_end_pos logic due to layout mismatch
        last_handler = (excepthandler_ty) asdl_seq_GET(handlers, n_except - 1);
        end_lineno = last_handler->end_lineno;
        end_col_offset = last_handler->end_col_offset;
    }
    return Try(body, handlers, orelse, finally, LINENO(n), n->n_col_offset,
               end_lineno, end_col_offset, c->c_arena);
}