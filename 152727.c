AsyncWith(asdl_seq * items, asdl_seq * body, string type_comment, int lineno,
          int col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = AsyncWith_kind;
    p->v.AsyncWith.items = items;
    p->v.AsyncWith.body = body;
    p->v.AsyncWith.type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}