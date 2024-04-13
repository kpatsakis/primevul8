ExceptHandler(expr_ty type, identifier name, asdl_seq * body, int lineno, int
              col_offset, PyArena *arena)
{
    excepthandler_ty p;
    p = (excepthandler_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = ExceptHandler_kind;
    p->v.ExceptHandler.type = type;
    p->v.ExceptHandler.name = name;
    p->v.ExceptHandler.body = body;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}