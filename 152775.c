Yield(expr_ty value, int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Yield_kind;
    p->v.Yield.value = value;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}