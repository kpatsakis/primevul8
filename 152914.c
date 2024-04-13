ast_for_decorator(struct compiling *c, const node *n)
{
    /* decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE */
    expr_ty d = NULL;
    expr_ty name_expr;

    REQ(n, decorator);
    REQ(CHILD(n, 0), AT);
    REQ(RCHILD(n, -1), NEWLINE);

    name_expr = ast_for_dotted_name(c, CHILD(n, 1));
    if (!name_expr)
        return NULL;

    if (NCH(n) == 3) { /* No arguments */
        d = name_expr;
        name_expr = NULL;
    }
    else if (NCH(n) == 5) { /* Call with no arguments */
        d = Call(name_expr, NULL, NULL, NULL, NULL, LINENO(n),
                 n->n_col_offset, c->c_arena);
        if (!d)
            return NULL;
        name_expr = NULL;
    }
    else {
        d = ast_for_call(c, CHILD(n, 3), name_expr);
        if (!d)
            return NULL;
        name_expr = NULL;
    }

    return d;
}