make_str_node_and_del(PyObject **str, struct compiling *c, const node* n)
{
    PyObject *s = *str;
    *str = NULL;
    assert(PyUnicode_CheckExact(s));
    if (PyArena_AddPyObject(c->c_arena, s) < 0) {
        Py_DECREF(s);
        return NULL;
    }
    return Constant(s, LINENO(n), n->n_col_offset,
                    n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}