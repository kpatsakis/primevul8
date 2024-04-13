SetComp(expr_ty elt, asdl_seq * generators, int lineno, int col_offset, int
        end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!elt) {
        PyErr_SetString(PyExc_ValueError,
                        "field elt is required for SetComp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = SetComp_kind;
    p->v.SetComp.elt = elt;
    p->v.SetComp.generators = generators;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}