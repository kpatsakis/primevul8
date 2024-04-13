With(asdl_seq * items, asdl_seq * body, string type_comment, int lineno, int
     col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = With_kind;
    p->v.With.items = items;
    p->v.With.body = body;
    p->v.With.type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}