Ta3Parser_ParseFileFlagsEx(FILE *fp, const char *filename,
                          const char *enc, grammar *g, int start,
                          const char *ps1, const char *ps2,
                          perrdetail *err_ret, int *flags)
{
    node *n;
    PyObject *fileobj = NULL;
#ifndef PGEN
    if (filename != NULL) {
        fileobj = PyUnicode_DecodeFSDefault(filename);
        if (fileobj == NULL) {
            err_ret->error = E_ERROR;
            return NULL;
        }
    }
#endif
    n = Ta3Parser_ParseFileObject(fp, fileobj, enc, g,
                                 start, ps1, ps2, err_ret, flags);
#ifndef PGEN
    Py_XDECREF(fileobj);
#endif
    return n;
}