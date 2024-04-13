Ellipsis(int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Ellipsis_kind;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}