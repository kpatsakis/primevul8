Dict(asdl_seq * keys, asdl_seq * values, int lineno, int col_offset, PyArena
     *arena)
{
    expr_ty p;
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Dict_kind;
    p->v.Dict.keys = keys;
    p->v.Dict.values = values;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}