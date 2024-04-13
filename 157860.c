DictComp(expr_ty key, expr_ty value, asdl_seq * generators, int lineno, int
         col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!key) {
        PyErr_SetString(PyExc_ValueError,
                        "field key is required for DictComp");
        return NULL;
    }
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for DictComp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = DictComp_kind;
    p->v.DictComp.key = key;
    p->v.DictComp.value = value;
    p->v.DictComp.generators = generators;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}