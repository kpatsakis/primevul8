Raise(expr_ty exc, expr_ty cause, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Raise_kind;
    p->v.Raise.exc = exc;
    p->v.Raise.cause = cause;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}