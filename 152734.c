Try(asdl_seq * body, asdl_seq * handlers, asdl_seq * orelse, asdl_seq *
    finalbody, int lineno, int col_offset, PyArena *arena)
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
    return p;
}