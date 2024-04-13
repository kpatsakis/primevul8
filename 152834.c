Set(asdl_seq * elts, int lineno, int col_offset, PyArena *arena)
{
    expr_ty p;
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Set_kind;
    p->v.Set.elts = elts;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}