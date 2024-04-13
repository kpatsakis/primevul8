comprehension(expr_ty target, expr_ty iter, asdl_seq * ifs, int is_async,
              PyArena *arena)
{
    comprehension_ty p;
    if (!target) {
        PyErr_SetString(PyExc_ValueError,
                        "field target is required for comprehension");
        return NULL;
    }
    if (!iter) {
        PyErr_SetString(PyExc_ValueError,
                        "field iter is required for comprehension");
        return NULL;
    }
    p = (comprehension_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->target = target;
    p->iter = iter;
    p->ifs = ifs;
    p->is_async = is_async;
    return p;
}