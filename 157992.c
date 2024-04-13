NamedExpr(expr_ty target, expr_ty value, int lineno, int col_offset, int
          end_lineno, int end_col_offset, PyArena *arena)
{
    expr_ty p;
    if (!target) {
        PyErr_SetString(PyExc_ValueError,
                        "field target is required for NamedExpr");
        return NULL;
    }
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for NamedExpr");
        return NULL;
    }
    p = (expr_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = NamedExpr_kind;
    p->v.NamedExpr.target = target;
    p->v.NamedExpr.value = value;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}