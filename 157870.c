Attribute(expr_ty value, identifier attr, expr_context_ty ctx, int lineno, int
          col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for Attribute");
        return NULL;
    }
    if (!attr) {
        PyErr_SetString(PyExc_ValueError,
                        "field attr is required for Attribute");
        return NULL;
    }
    if (!ctx) {
        PyErr_SetString(PyExc_ValueError,
                        "field ctx is required for Attribute");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Attribute_kind;
    p->v.Attribute.value = value;
    p->v.Attribute.attr = attr;
    p->v.Attribute.ctx = ctx;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}