NameConstant(singleton value, int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for NameConstant");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = NameConstant_kind;
    p->v.NameConstant.value = value;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}