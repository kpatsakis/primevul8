ast_for_with_item(struct compiling *c, const node *n, asdl_seq *content, string type_comment)
{
    expr_ty context_expr, optional_vars = NULL;

    REQ(n, with_item);
    context_expr = ast_for_expr(c, CHILD(n, 0));
    if (!context_expr)
        return NULL;
    if (NCH(n) == 3) {
        optional_vars = ast_for_expr(c, CHILD(n, 2));

        if (!optional_vars) {
            return NULL;
        }
        if (!set_context(c, optional_vars, Store, n)) {
            return NULL;
        }
    }

    return With(context_expr, optional_vars, content, type_comment, LINENO(n),
                n->n_col_offset, c->c_arena);
}