JoinedStr(asdl_seq * values, int lineno, int col_offset, int end_lineno, int
          end_col_offset, PyArena *arena)
{
    expr_ty p;
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = JoinedStr_kind;
    p->v.JoinedStr.values = values;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}