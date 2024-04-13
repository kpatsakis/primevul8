If(expr_ty test, asdl_seq * body, asdl_seq * orelse, int lineno, int
   col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!test) {
        PyErr_SetString(PyExc_ValueError,
                        "field test is required for If");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = If_kind;
    p->v.If.test = test;
    p->v.If.body = body;
    p->v.If.orelse = orelse;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}