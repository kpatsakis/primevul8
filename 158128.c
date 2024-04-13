Starred(expr_ty value, expr_context_ty ctx, int lineno, int col_offset, int
        end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for Starred");
        return NULL;
    }
    if (!ctx) {
        PyErr_SetString(PyExc_ValueError,
                        "field ctx is required for Starred");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Starred_kind;
    p->v.Starred.value = value;
    p->v.Starred.ctx = ctx;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}