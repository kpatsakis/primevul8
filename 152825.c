Expression(expr_ty body, PyArena *arena)
{
    mod_ty p;
    if (!body) {
        PyErr_SetString(PyExc_ValueError,
                        "field body is required for Expression");
        return NULL;
    }
    p = (mod_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Expression_kind;
    p->v.Expression.body = body;
    return p;
}