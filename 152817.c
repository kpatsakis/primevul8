Expr(expr_ty value, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for Expr");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Expr_kind;
    p->v.Expr.value = value;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}