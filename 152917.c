Index(expr_ty value, PyArena *arena)
{
    slice_ty p;
    if (!value) {
        PyErr_SetString(PyExc_ValueError,
                        "field value is required for Index");
        return NULL;
    }
    p = (slice_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->kind = Index_kind;
    p->v.Index.value = value;
    return p;
}