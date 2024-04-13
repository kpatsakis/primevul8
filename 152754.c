ast_for_try_stmt(struct compiling *c, const node *n)
{
    const int nch = NCH(n);
    int n_except = (nch - 3)/3;
    asdl_seq *body, *orelse = NULL, *finally = NULL;

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
        ast_error(n, "malformed 'try' statement");
        return NULL;
    }

    if (n_except > 0) {
        int i;
        stmt_ty except_st;
        /* process except statements to create a try ... except */
        asdl_seq *handlers = asdl_seq_new(n_except, c->c_arena);
        if (handlers == NULL)
            return NULL;

        for (i = 0; i < n_except; i++) {
            excepthandler_ty e = ast_for_except_clause(c, CHILD(n, 3 + i * 3),
                                                       CHILD(n, 5 + i * 3));
            if (!e)
                return NULL;
            asdl_seq_SET(handlers, i, e);
        }

        except_st = TryExcept(body, handlers, orelse, LINENO(n),
                              n->n_col_offset, c->c_arena);
        if (!finally)
            return except_st;

        /* if a 'finally' is present too, we nest the TryExcept within a
           TryFinally to emulate try ... except ... finally */
        body = asdl_seq_new(1, c->c_arena);
        if (body == NULL)
            return NULL;
        asdl_seq_SET(body, 0, except_st);
    }

    /* must be a try ... finally (except clauses are in body, if any exist) */
    assert(finally != NULL);
    return TryFinally(body, finally, LINENO(n), n->n_col_offset, c->c_arena);
}