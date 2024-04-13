Subscript(expr_ty value, slice_ty slice, expr_context_ty ctx, int lineno, int
          col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for Subscript");
        return NULL;
    }
    if (!slice) {
        PyErr_SetString(PyExc_ValueError,
                        "field slice is required for Subscript");
        return NULL;
    }
    if (!ctx) {
        PyErr_SetString(PyExc_ValueError,
                        "field ctx is required for Subscript");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Subscript_kind;
    p->v.Subscript.value = value;
    p->v.Subscript.slice = slice;
    p->v.Subscript.ctx = ctx;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}