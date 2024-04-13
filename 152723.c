Num(object n, int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    if (!n) {
        PyErr_SetString(PyExc_ValueError,
                        "field n is required for Num");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Num_kind;
    p->v.Num.n = n;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}