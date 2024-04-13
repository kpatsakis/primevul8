List(asdl_seq * elts, expr_context_ty ctx, int lineno, int col_offset, PyArena
     *arena)
{
    expr_ty p;
    if (!ctx) {
        PyErr_SetString(PyExc_ValueError,
                        "field ctx is required for List");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = List_kind;
    p->v.List.elts = elts;
    p->v.List.ctx = ctx;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}