ast_for_del_stmt(struct compiling *c, const node *n)
{
    asdl_seq *expr_list;

    /* del_stmt: 'del' exprlist */
    REQ(n, del_stmt);

    expr_list = ast_for_exprlist(c, CHILD(n, 1), Del);
    if (!expr_list)
        return NULL;
    return Delete(expr_list, LINENO(n), n->n_col_offset,
                  n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}