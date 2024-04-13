ImportFrom(identifier module, asdl_seq * names, int level, int lineno, int
           col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = ImportFrom_kind;
    p->v.ImportFrom.module = module;
    p->v.ImportFrom.names = names;
    p->v.ImportFrom.level = level;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}