ast_for_exec_stmt(struct compiling *c, const node *n)
{
    expr_ty expr1, globals = NULL, locals = NULL;
    int n_children = NCH(n);
    if (n_children != 2 && n_children != 4 && n_children != 6) {
        PyErr_Format(PyExc_SystemError,
                     "poorly formed 'exec' statement: %d parts to statement",
                     n_children);
        return NULL;
    }

    /* exec_stmt: 'exec' expr ['in' test [',' test]] */
    REQ(n, exec_stmt);
    expr1 = ast_for_expr(c, CHILD(n, 1));
    if (!expr1)
        return NULL;

    if (expr1->kind == Tuple_kind && n_children < 4 &&
        (asdl_seq_LEN(expr1->v.Tuple.elts) == 2 ||
         asdl_seq_LEN(expr1->v.Tuple.elts) == 3)) {
        /* Backwards compatibility: passing exec args as a tuple */
        globals = asdl_seq_GET(expr1->v.Tuple.elts, 1);
        if (asdl_seq_LEN(expr1->v.Tuple.elts) == 3) {
            locals = asdl_seq_GET(expr1->v.Tuple.elts, 2);
        }
        expr1 = asdl_seq_GET(expr1->v.Tuple.elts, 0);
    }

    if (n_children >= 4) {
        globals = ast_for_expr(c, CHILD(n, 3));
        if (!globals)
            return NULL;
    }
    if (n_children == 6) {
        locals = ast_for_expr(c, CHILD(n, 5));
        if (!locals)
            return NULL;
    }

    return Exec(expr1, globals, locals, LINENO(n), n->n_col_offset,
                c->c_arena);
}