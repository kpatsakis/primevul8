For(expr_ty target, expr_ty iter, asdl_seq * body, asdl_seq * orelse, string
    type_comment, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!target) {
        PyErr_SetString(PyExc_ValueError,
                        "field target is required for For");
        return NULL;
    }
    if (!iter) {
        PyErr_SetString(PyExc_ValueError,
                        "field iter is required for For");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = For_kind;
    p->v.For.target = target;
    p->v.For.iter = iter;
    p->v.For.body = body;
    p->v.For.orelse = orelse;
    p->v.For.type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}