keyword(identifier arg, expr_ty value, PyArena *arena)
{
    keyword_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for keyword");
        return NULL;
    }
    p = (keyword_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->arg = arg;
    p->value = value;
    return p;
}