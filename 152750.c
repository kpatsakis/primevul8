alias_for_import_name(struct compiling *c, const node *n, int store)
{
    /*
      import_as_name: NAME ['as' NAME]
      dotted_as_name: dotted_name ['as' NAME]
      dotted_name: NAME ('.' NAME)*
    */
    PyObject *str, *name;

 loop:
    switch (TYPE(n)) {
         case import_as_name: {
            node *name_node = CHILD(n, 0);
            str = NULL;
            if (NCH(n) == 3) {
                node *str_node = CHILD(n, 2);
                if (store && !forbidden_check(c, str_node, STR(str_node)))
                    return NULL;
                str = NEW_IDENTIFIER(str_node);
                if (!str)
                    return NULL;
            }
            else {
                if (!forbidden_check(c, name_node, STR(name_node)))
                    return NULL;
            }
            name = NEW_IDENTIFIER(name_node);
            if (!name)
                return NULL;
            return alias(name, str, c->c_arena);
        }
        case dotted_as_name:
            if (NCH(n) == 1) {
                n = CHILD(n, 0);
                goto loop;
            }
            else {
                node *asname_node = CHILD(n, 2);
                alias_ty a = alias_for_import_name(c, CHILD(n, 0), 0);
                if (!a)
                    return NULL;
                assert(!a->asname);
                if (!forbidden_check(c, asname_node, STR(asname_node)))
                    return NULL;
                a->asname = NEW_IDENTIFIER(asname_node);
                if (!a->asname)
                    return NULL;
                return a;
            }
            break;
        case dotted_name:
            if (NCH(n) == 1) {
                node *name_node = CHILD(n, 0);
                if (store && !forbidden_check(c, name_node, STR(name_node)))
                    return NULL;
                name = NEW_IDENTIFIER(name_node);
                if (!name)
                    return NULL;
                return alias(name, NULL, c->c_arena);
            }
            else {
                /* Create a string of the form "a.b.c" */
                int i;
                size_t len;
                char *s;
                PyObject *uni;

                len = 0;
                for (i = 0; i < NCH(n); i += 2)
                    /* length of string plus one for the dot */
                    len += strlen(STR(CHILD(n, i))) + 1;
                len--; /* the last name doesn't have a dot */
                str = PyBytes_FromStringAndSize(NULL, len);
                if (!str)
                    return NULL;
                s = PyBytes_AS_STRING(str);
                if (!s)
                    return NULL;
                for (i = 0; i < NCH(n); i += 2) {
                    char *sch = STR(CHILD(n, i));
                    strcpy(s, STR(CHILD(n, i)));
                    s += strlen(sch);
                    *s++ = '.';
                }
                --s;
                *s = '\0';
                uni = PyUnicode_DecodeUTF8(PyBytes_AS_STRING(str),
                                           PyBytes_GET_SIZE(str),
                                           NULL);
                Py_DECREF(str);
                if (!uni)
                    return NULL;
                str = uni;
                PyUnicode_InternInPlace(&str);
                if (PyArena_AddPyObject(c->c_arena, str) < 0) {
                    Py_DECREF(str);
                    return NULL;
                }
                return alias(str, NULL, c->c_arena);
            }
            break;
        case STAR:
            str = PyUnicode_InternFromString("*");
            if (PyArena_AddPyObject(c->c_arena, str) < 0) {
                Py_DECREF(str);
                return NULL;
            }
            return alias(str, NULL, c->c_arena);
        default:
            PyErr_Format(PyExc_SystemError,
                         "unexpected import name: %d", TYPE(n));
            return NULL;
    }

    PyErr_SetString(PyExc_SystemError, "unhandled import name condition");
    return NULL;
}