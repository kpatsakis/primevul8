PyInit_parser(void)
{
    PyObject *module, *copyreg;

    if (PyType_Ready(&PyST_Type) < 0)
        return NULL;
    module = PyModule_Create(&parsermodule);
    if (module == NULL)
        return NULL;

    if (parser_error == 0)
        parser_error = PyErr_NewException("parser.ParserError", NULL, NULL);

    if (parser_error == 0)
        return NULL;
    /* CAUTION:  The code next used to skip bumping the refcount on
     * parser_error.  That's a disaster if PyInit_parser() gets called more
     * than once.  By incref'ing, we ensure that each module dict that
     * gets created owns its reference to the shared parser_error object,
     * and the file static parser_error vrbl owns a reference too.
     */
    Py_INCREF(parser_error);
    if (PyModule_AddObject(module, "ParserError", parser_error) != 0)
        return NULL;

    Py_INCREF(&PyST_Type);
    PyModule_AddObject(module, "STType", (PyObject*)&PyST_Type);

    PyModule_AddStringConstant(module, "__copyright__",
                               parser_copyright_string);
    PyModule_AddStringConstant(module, "__doc__",
                               parser_doc_string);
    PyModule_AddStringConstant(module, "__version__",
                               parser_version_string);

    /* Register to support pickling.
     * If this fails, the import of this module will fail because an
     * exception will be raised here; should we clear the exception?
     */
    copyreg = PyImport_ImportModuleNoBlock("copyreg");
    if (copyreg != NULL) {
        PyObject *func, *pickler;
        _Py_IDENTIFIER(pickle);
        _Py_IDENTIFIER(sequence2st);
        _Py_IDENTIFIER(_pickler);

        func = _PyObject_GetAttrId(copyreg, &PyId_pickle);
        pickle_constructor = _PyObject_GetAttrId(module, &PyId_sequence2st);
        pickler = _PyObject_GetAttrId(module, &PyId__pickler);
        Py_XINCREF(pickle_constructor);
        if ((func != NULL) && (pickle_constructor != NULL)
            && (pickler != NULL)) {
            PyObject *res;

            res = PyObject_CallFunctionObjArgs(func, &PyST_Type, pickler,
                                               pickle_constructor, NULL);
            Py_XDECREF(res);
        }
        Py_XDECREF(func);
        Py_XDECREF(pickle_constructor);
        Py_XDECREF(pickler);
        Py_DECREF(copyreg);
    }
    return module;
}