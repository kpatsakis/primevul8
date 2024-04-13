ast3_parse(PyObject *self, PyObject *args)
{
    PyObject *return_value = NULL;
    PyObject *source;
    PyObject *filename;
    const char *mode;
    int feature_version;

    if (PyArg_ParseTuple(args, "OO&si:parse", &source, PyUnicode_FSDecoder, &filename, &mode, &feature_version))
        return_value = ast3_parse_impl(source, filename, mode, feature_version);

    return return_value;
}