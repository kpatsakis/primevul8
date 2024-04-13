Pass(int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Pass_kind;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}