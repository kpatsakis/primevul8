Lambda(arguments_ty args, expr_ty body, int lineno, int col_offset, int
       end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!args) {
        PyErr_SetString(PyExc_ValueError,
                        "field args is required for Lambda");
        return NULL;
    }
    if (!body) {
        PyErr_SetString(PyExc_ValueError,
                        "field body is required for Lambda");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Lambda_kind;
    p->v.Lambda.args = args;
    p->v.Lambda.body = body;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}