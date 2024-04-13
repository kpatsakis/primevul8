fp_setreadl(struct tok_state *tok, const char* enc)
{
    PyObject *readline, *io, *stream;
    _Py_IDENTIFIER(open);
    _Py_IDENTIFIER(readline);
    int fd;
    long pos;

    fd = fileno(tok->fp);
    /* Due to buffering the file offset for fd can be different from the file
     * position of tok->fp.  If tok->fp was opened in text mode on Windows,
     * its file position counts CRLF as one char and can't be directly mapped
     * to the file offset for fd.  Instead we step back one byte and read to
     * the end of line.*/
    pos = ftell(tok->fp);
    if (pos == -1 ||
        lseek(fd, (off_t)(pos > 0 ? pos - 1 : pos), SEEK_SET) == (off_t)-1) {
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, NULL);
        return 0;
    }

    io = PyImport_ImportModuleNoBlock("io");
    if (io == NULL)
        return 0;

    stream = _PyObject_CallMethodId(io, &PyId_open, "isisOOO",
                    fd, "r", -1, enc, Py_None, Py_None, Py_False);
    Py_DECREF(io);
    if (stream == NULL)
        return 0;

    readline = _PyObject_GetAttrId(stream, &PyId_readline);
    Py_DECREF(stream);
    if (readline == NULL)
        return 0;
    Py_XSETREF(tok->decoding_readline, readline);

    if (pos > 0) {
        PyObject *bufobj = _PyObject_CallNoArg(readline);
        if (bufobj == NULL)
            return 0;
        Py_DECREF(bufobj);
    }

    return 1;
}