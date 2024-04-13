ast_for_namedexpr(struct compiling *c, const node *n)
{
    /* if_stmt: 'if' namedexpr_test ':' suite ('elif' namedexpr_test ':' suite)*
         ['else' ':' suite]
       namedexpr_test: test [':=' test]
       argument: ( test [comp_for] |
            test ':=' test |
            test '=' test |
            '**' test |
            '*' test )
    */
    expr_ty target, value;

    target = ast_for_expr(c, CHILD(n, 0));
    if (!target)
        return NULL;

    value = ast_for_expr(c, CHILD(n, 2));
    if (!value)
        return NULL;

    if (!set_context(c, target, NamedStore, n))
        return NULL;

    return NamedExpr(target, value, LINENO(n), n->n_col_offset, n->n_end_lineno,
                     n->n_end_col_offset, c->c_arena);
}