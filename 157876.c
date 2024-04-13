Assert(expr_ty test, expr_ty msg, int lineno, int col_offset, int end_lineno,
       int end_col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!test) {
        PyErr_SetString(PyExc_ValueError,
                        "field test is required for Assert");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Assert_kind;
    p->v.Assert.test = test;
    p->v.Assert.msg = msg;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}