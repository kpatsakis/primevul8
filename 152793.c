Str(string s, string kind, int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    if (!s) {
        PyErr_SetString(PyExc_ValueError,
                        "field s is required for Str");
        return NULL;
    }
    if (!kind) {
        PyErr_SetString(PyExc_ValueError,
                        "field kind is required for Str");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Str_kind;
    p->v.Str.s = s;
    p->v.Str.kind = kind;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}