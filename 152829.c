Ta3Parser_ParseStringFlagsFilenameEx(const char *s, const char *filename_str,
                          grammar *g, int start,
                          perrdetail *err_ret, int *flags)
{
    node *n;
    PyObject *filename = NULL;
#ifndef PGEN
    if (filename_str != NULL) {
        filename = PyUnicode_DecodeFSDefault(filename_str);
        if (filename == NULL) {
            err_ret->error = E_ERROR;
            return NULL;
        }
    }
#endif
    n = Ta3Parser_ParseStringObject(s, filename, g, start, err_ret, flags);
#ifndef PGEN
    Py_XDECREF(filename);
#endif
    return n;
}