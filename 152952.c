fstring_compile_expr(const char *expr_start, const char *expr_end,
                     struct compiling *c, const node *n)

{
    PyCompilerFlags cf;
    node *mod_n;
    mod_ty mod;
    char *str;
    Py_ssize_t len;
    const char *s;
    PyObject *fstring_name;

    assert(expr_end >= expr_start);
    assert(*(expr_start-1) == '{');
    assert(*expr_end == '}' || *expr_end == '!' || *expr_end == ':');

    /* If the substring is all whitespace, it's an error.  We need to catch this
       here, and not when we call PyParser_SimpleParseStringFlagsFilename,
       because turning the expression '' in to '()' would go from being invalid
       to valid. */
    for (s = expr_start; s != expr_end; s++) {
        char c = *s;
        /* The Python parser ignores only the following whitespace
           characters (\r already is converted to \n). */
        if (!(c == ' ' || c == '\t' || c == '\n' || c == '\f')) {
            break;
        }
    }
    if (s == expr_end) {
        ast_error(c, n, "f-string: empty expression not allowed");
        return NULL;
    }

    len = expr_end - expr_start;
    /* Allocate 3 extra bytes: open paren, close paren, null byte. */
    str = PyMem_RawMalloc(len + 3);
    if (str == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    str[0] = '(';
    memcpy(str+1, expr_start, len);
    str[len+1] = ')';
    str[len+2] = 0;

    cf.cf_flags = PyCF_ONLY_AST;
    mod_n = PyParser_SimpleParseStringFlagsFilename(str, "<fstring>",
                                                    Py_eval_input, 0);
    if (!mod_n) {
        PyMem_RawFree(str);
        return NULL;
    }
    /* Reuse str to find the correct column offset. */
    str[0] = '{';
    str[len+1] = '}';
    fstring_fix_node_location(n, mod_n, str);
    fstring_name = PyUnicode_FromString("<fstring>");
    mod = string_object_to_c_ast(str, fstring_name,
                                 Py_eval_input, &cf,
                                 c->c_feature_version, c->c_arena);
    Py_DECREF(fstring_name);
    PyMem_RawFree(str);
    Ta3Node_Free(mod_n);
    if (!mod)
        return NULL;
    return mod->v.Expression.body;
}