Return(expr_ty value, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Return_kind;
    p->v.Return.value = value;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}