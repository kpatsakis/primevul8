Nonlocal(asdl_seq * names, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Nonlocal_kind;
    p->v.Nonlocal.names = names;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}