Name(identifier id, expr_context_ty ctx, int lineno, int col_offset, int
     end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!id) {
        PyErr_SetString(PyExc_ValueError,
                        "field id is required for Name");
        return NULL;
    }
    if (!ctx) {
        PyErr_SetString(PyExc_ValueError,
                        "field ctx is required for Name");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Name_kind;
    p->v.Name.id = id;
    p->v.Name.ctx = ctx;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}