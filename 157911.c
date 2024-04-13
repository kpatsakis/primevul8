Try(asdl_seq * body, asdl_seq * handlers, asdl_seq * orelse, asdl_seq *
    finalbody, int lineno, int col_offset, int end_lineno, int end_col_offset,
    PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Try_kind;
    p->v.Try.body = body;
    p->v.Try.handlers = handlers;
    p->v.Try.orelse = orelse;
    p->v.Try.finalbody = finalbody;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}