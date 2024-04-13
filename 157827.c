ast_for_lambdef(struct compiling *c, const node *n)
{
    /* lambdef: 'lambda' [varargslist] ':' test
       lambdef_nocond: 'lambda' [varargslist] ':' test_nocond */
    arguments_ty args;
    expr_ty expression;

    if (NCH(n) == 3) {
        args = arguments(NULL, NULL, NULL, NULL, NULL, NULL, c->c_arena);
        if (!args)
            return NULL;
        expression = ast_for_expr(c, CHILD(n, 2));
        if (!expression)
            return NULL;
    }
    else {
        args = ast_for_arguments(c, CHILD(n, 1));
        if (!args)
            return NULL;
        expression = ast_for_expr(c, CHILD(n, 3));
        if (!expression)
            return NULL;
    }

    return Lambda(args, expression, LINENO(n), n->n_col_offset,
                  n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}