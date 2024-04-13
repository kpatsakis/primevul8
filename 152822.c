arg(identifier arg, expr_ty annotation, string type_comment, int lineno, int
    col_offset, PyArena *arena)
{
    arg_ty p;
    if (!arg) {
        PyErr_SetString(PyExc_ValueError,
                        "field arg is required for arg");
        return NULL;
    }
    p = (arg_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->arg = arg;
    p->annotation = annotation;
    p->type_comment = type_comment;
    p->lineno = lineno;
    p->col_offset = col_offset;
    return p;
}