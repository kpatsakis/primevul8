Delete(asdl_seq * targets, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Delete_kind;
    p->v.Delete.targets = targets;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}