AsyncFor(expr_ty target, expr_ty iter, asdl_seq * body, asdl_seq * orelse,
         string type_comment, int lineno, int col_offset, PyArena *arena)
{
    stmt_ty p;
    if (!target) {
        PyErr_SetString(PyExc_ValueError,
                        "field target is required for AsyncFor");
        return NULL;
    }
    if (!iter) {
        PyErr_SetString(PyExc_ValueError,
                        "field iter is required for AsyncFor");
        return NULL;
    }
    p = (stmt_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = AsyncFor_kind;
    p->v.AsyncFor.target = target;
    p->v.AsyncFor.iter = iter;
    p->v.AsyncFor.body = body;
    p->v.AsyncFor.orelse = orelse;
    p->v.AsyncFor.type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}