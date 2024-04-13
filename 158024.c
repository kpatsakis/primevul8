set_main_loader(PyObject *d, const char *filename, const char *loader_name)
{
    PyObject *filename_obj, *bootstrap, *loader_type = NULL, *loader;
    int result = 0;

    filename_obj = PyUnicode_DecodeFSDefault(filename);
    if (filename_obj == NULL)
        return -1;
    PyInterpreterState *interp = _PyInterpreterState_Get();
    bootstrap = PyObject_GetAttrString(interp->importlib,
                                       "_bootstrap_external");
    if (bootstrap != NULL) {
        loader_type = PyObject_GetAttrString(bootstrap, loader_name);
        Py_DECREF(bootstrap);
    }
    if (loader_type == NULL) {
        Py_DECREF(filename_obj);
        return -1;
    }
    loader = PyObject_CallFunction(loader_type, "sN", "__main__", filename_obj);
    Py_DECREF(loader_type);
    if (loader == NULL) {
        return -1;
    }
    if (PyDict_SetItemString(d, "__loader__", loader) < 0) {
        result = -1;
    }
    Py_DECREF(loader);
    return result;
}