AnnAssign(expr_ty target, expr_ty annotation, expr_ty value, int simple, int
          lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!target) {
        PyErr_SetString(PyExc_ValueError,
                        "field target is required for AnnAssign");
        return NULL;
    }
    if (!annotation) {
        PyErr_SetString(PyExc_ValueError,
                        "field annotation is required for AnnAssign");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = AnnAssign_kind;
    p->v.AnnAssign.target = target;
    p->v.AnnAssign.annotation = annotation;
    p->v.AnnAssign.value = value;
    p->v.AnnAssign.simple = simple;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}