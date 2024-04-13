FormattedValue(expr_ty value, int conversion, expr_ty format_spec, int lineno,
               int col_offset, PyArena *arena)
{
    expr_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for FormattedValue");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = FormattedValue_kind;
    p->v.FormattedValue.value = value;
    p->v.FormattedValue.conversion = conversion;
    p->v.FormattedValue.format_spec = format_spec;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}