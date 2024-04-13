Bytes(bytes s, int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    if (!s) {
        PyErr_SetString(PyExc_ValueError,
                        "field s is required for Bytes");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Bytes_kind;
    p->v.Bytes.s = s;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}