ast3_parse_impl(PyObject *source,
                PyObject *filename,
                const char *mode,
                int feature_version)
{
    PyObject *source_copy;
    const char *str;
    int compile_mode = -1;
    PyCompilerFlags cf;
    int start[] = {Py_file_input, Py_eval_input, Py_single_input, Py_func_type_input};
    PyObject *result;

    cf.cf_flags = PyCF_ONLY_AST | PyCF_SOURCE_IS_UTF8;

    if (strcmp(mode, "exec") == 0)
        compile_mode = 0;
    else if (strcmp(mode, "eval") == 0)
        compile_mode = 1;
    else if (strcmp(mode, "single") == 0)
        compile_mode = 2;
    else if (strcmp(mode, "func_type") == 0)
        compile_mode = 3;
    else {
        PyErr_SetString(PyExc_ValueError,
                        "parse() mode must be 'exec', 'eval', 'single', for 'func_type'");
        goto error;
    }

    str = source_as_string(source, "parse", "string or bytes", &cf, &source_copy);
    if (str == NULL)
        goto error;

    result = string_object_to_py_ast(str, filename, start[compile_mode], &cf, feature_version);
    Py_XDECREF(source_copy);
    goto finally;

error:
    result = NULL;
finally:
    Py_DECREF(filename);
    return result;
}