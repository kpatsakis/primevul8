Tuple(asdl_seq * elts, expr_context_ty ctx, int lineno, int col_offset, PyArena
      *arena)
{
    expr_ty p;
    if (!ctx) {
        PyErr_SetString(PyExc_ValueError,
                        "field ctx is required for Tuple");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Tuple_kind;
    p->v.Tuple.elts = elts;
    p->v.Tuple.ctx = ctx;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}