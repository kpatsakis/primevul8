new_identifier(const char* n, PyArena *arena) {
    PyObject* id = PyUnicode_InternFromString(n);
    if (id != NULL)
        PyArena_AddPyObject(arena, id);
    return id;
}