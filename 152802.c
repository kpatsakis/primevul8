alias(identifier name, identifier asname, PyArena *arena)
{
    alias_ty p;
    if (!name) {
        PyErr_SetString(PyExc_ValueError,
                        "field name is required for alias");
        return NULL;
    }
    p = (alias_ty)PyArena_Malloc(arena, sizeof(*p));
    if (!p)
        return NULL;
    p->name = name;
    p->asname = asname;
    return p;
}