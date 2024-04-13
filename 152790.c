make_str_node_and_del(PyObject **str, struct compiling *c, const node* n)
{
    PyObject *kind, *s = *str;
    const char *raw = STR(CHILD(n, 0));
    /* currently Python allows up to 2 string modifiers */
    char *ch, s_kind[3] = {0, 0, 0};
    ch = s_kind;
    while (*raw && *raw != '\'' && *raw != '"') {
        *ch++ = *raw++;
    }
    kind = PyUnicode_FromString(s_kind);
    if (!kind) {
        return NULL;
    }
    *str = NULL;
    assert(PyUnicode_CheckExact(s));
    if (PyArena_AddPyObject(c->c_arena, s) < 0) {
        Py_DECREF(s);
        return NULL;
    }
    return Str(s, kind, LINENO(n), n->n_col_offset, c->c_arena);
}