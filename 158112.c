While(expr_ty test, asdl_seq * body, asdl_seq * orelse, int lineno, int
      col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!test) {
        PyErr_SetString(PyExc_ValueError,
                        "field test is required for While");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = While_kind;
    p->v.While.test = test;
    p->v.While.body = body;
    p->v.While.orelse = orelse;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}