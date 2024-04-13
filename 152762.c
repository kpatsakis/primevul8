IfExp(expr_ty test, expr_ty body, expr_ty orelse, int lineno, int col_offset,
      PyArena *arena)
{
    expr_ty p;
    if (!test) {
        PyErr_SetString(PyExc_ValueError,
                        "field test is required for IfExp");
        return NULL;
    }
    if (!body) {
        PyErr_SetString(PyExc_ValueError,
                        "field body is required for IfExp");
        return NULL;
    }
    if (!orelse) {
        PyErr_SetString(PyExc_ValueError,
                        "field orelse is required for IfExp");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = IfExp_kind;
    p->v.IfExp.test = test;
    p->v.IfExp.body = body;
    p->v.IfExp.orelse = orelse;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}