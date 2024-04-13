AugAssign(expr_ty target, operator_ty op, expr_ty value, int lineno, int
          col_offset, int end_lineno, int end_col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!target) {
        PyErr_SetString(PyExc_ValueError,
                        "field target is required for AugAssign");
        return NULL;
    }
    if (!op) {
        PyErr_SetString(PyExc_ValueError,
                        "field op is required for AugAssign");
        return NULL;
    }
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for AugAssign");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = AugAssign_kind;
    p->v.AugAssign.target = target;
    p->v.AugAssign.op = op;
    p->v.AugAssign.value = value;
    p->lineno = lineno;
    p->col_offset = col_offset;
    p->end_lineno = end_lineno;
    p->end_col_offset = end_col_offset;
    return p;
}