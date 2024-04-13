ast_for_factor(struct compiling *c, const node *n)
{
    expr_ty expression;

    expression = ast_for_expr(c, CHILD(n, 1));
    if (!expression)
        return NULL;

    switch (TYPE(CHILD(n, 0))) {
        case PLUS:
            return UnaryOp(UAdd, expression, LINENO(n), n->n_col_offset,
                           n->n_end_lineno, n->n_end_col_offset,
                           c->c_arena);
        case MINUS:
            return UnaryOp(USub, expression, LINENO(n), n->n_col_offset,
                           n->n_end_lineno, n->n_end_col_offset,
                           c->c_arena);
        case TILDE:
            return UnaryOp(Invert, expression, LINENO(n), n->n_col_offset,
                           n->n_end_lineno, n->n_end_col_offset,
                           c->c_arena);
    }
    PyErr_Format(PyExc_SystemError, "unhandled factor: %d",
                 TYPE(CHILD(n, 0)));
    return NULL;
}