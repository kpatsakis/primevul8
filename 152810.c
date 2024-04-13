ast_for_arguments(struct compiling *c, const node *n)
{
    /* parameters: '(' [varargslist] ')'
       varargslist: ((fpdef ['=' test] ',' [TYPE_COMMENT])*
                     ('*' NAME [',' [TYPE_COMMENT]  '**' NAME] [TYPE_COMMENT] | '**' NAME [TYPE_COMMENT]) |
                     fpdef ['=' test] (',' [TYPE_COMMENT] fpdef ['=' test])* [','] [TYPE_COMMENT])
    */
    int i, j, k, l, n_args = 0, n_all_args = 0, n_defaults = 0, found_default = 0;
    asdl_seq *args, *defaults, *type_comments = NULL;
    identifier vararg = NULL, kwarg = NULL;
    node *ch;

    if (TYPE(n) == parameters) {
        if (NCH(n) == 2) /* () as argument list */
            return arguments(NULL, NULL, NULL, NULL, NULL, c->c_arena);
        n = CHILD(n, 1);
    }
    REQ(n, varargslist);

    /* first count the number of normal args & defaults */
    for (i = 0; i < NCH(n); i++) {
        ch = CHILD(n, i);
        if (TYPE(ch) == fpdef)
            n_args++;
        if (TYPE(ch) == EQUAL)
            n_defaults++;
        if (TYPE(ch) == STAR || TYPE(ch) == DOUBLESTAR)
            n_all_args++;
    }
    n_all_args += n_args;
    args = (n_args ? asdl_seq_new(n_args, c->c_arena) : NULL);
    if (!args && n_args)
        return NULL;
    defaults = (n_defaults ? asdl_seq_new(n_defaults, c->c_arena) : NULL);
    if (!defaults && n_defaults)
        return NULL;
    /* type_comments will be lazily initialized if needed.  If there are no
       per-argument type comments, it will remain NULL.  Otherwise, it will be
       an asdl_seq with length equal to the number of args (including varargs
       and kwargs, if present) and with members set to the string of each arg's
       type comment, if present, or NULL otherwise.
     */

    /* fpdef: NAME | '(' fplist ')'
       fplist: fpdef (',' fpdef)* [',']
    */
    i = 0;
    j = 0;  /* index for defaults */
    k = 0;  /* index for args */
    l = 0;  /* index for type comments */
    while (i < NCH(n)) {
        ch = CHILD(n, i);
        switch (TYPE(ch)) {
            case fpdef: {
                int complex_args = 0, parenthesized = 0;
            handle_fpdef:
                /* XXX Need to worry about checking if TYPE(CHILD(n, i+1)) is
                   anything other than EQUAL or a comma? */
                /* XXX Should NCH(n) check be made a separate check? */
                if (i + 1 < NCH(n) && TYPE(CHILD(n, i + 1)) == EQUAL) {
                    expr_ty expression = ast_for_expr(c, CHILD(n, i + 2));
                    if (!expression)
                        return NULL;
                    assert(defaults != NULL);
                    asdl_seq_SET(defaults, j++, expression);
                    i += 2;
                    found_default = 1;
                }
                else if (found_default) {
                    /* def f((x)=4): pass should raise an error.
                       def f((x, (y))): pass will just incur the tuple unpacking warning. */
                    if (parenthesized && !complex_args) {
                        ast_error(n, "parenthesized arg with default");
                        return NULL;
                    }
                    ast_error(n,
                             "non-default argument follows default argument");
                    return NULL;
                }
                if (NCH(ch) == 3) {
                    ch = CHILD(ch, 1);
                    /* def foo((x)): is not complex, special case. */
                    if (NCH(ch) != 1) {
                        /* We have complex arguments, setup for unpacking. */
                        if (Py_Py3kWarningFlag && !ast_warn(c, ch,
                            "tuple parameter unpacking has been removed in 3.x"))
                            return NULL;
                        complex_args = 1;
                        asdl_seq_SET(args, k++, compiler_complex_args(c, ch));
                        if (!asdl_seq_GET(args, k-1))
                                return NULL;
                    } else {
                        /* def foo((x)): setup for checking NAME below. */
                        /* Loop because there can be many parens and tuple
                           unpacking mixed in. */
                        parenthesized = 1;
                        ch = CHILD(ch, 0);
                        assert(TYPE(ch) == fpdef);
                        goto handle_fpdef;
                    }
                }
                if (TYPE(CHILD(ch, 0)) == NAME) {
                    PyObject *id;
                    expr_ty name;
                    if (!forbidden_check(c, n, STR(CHILD(ch, 0))))
                        return NULL;
                    id = NEW_IDENTIFIER(CHILD(ch, 0));
                    if (!id)
                        return NULL;
                    name = Name(id, Param, LINENO(ch), ch->n_col_offset,
                                c->c_arena);
                    if (!name)
                        return NULL;
                    asdl_seq_SET(args, k++, name);

                }
                i += 1; /* the name */
                if (i < NCH(n) && TYPE(CHILD(n, i)) == COMMA)
                    i += 1; /* the comma, if present */
                if (parenthesized && Py_Py3kWarningFlag &&
                    !ast_warn(c, ch, "parenthesized argument names "
                              "are invalid in 3.x"))
                    return NULL;

                break;
            }
            case STAR:
                if (!forbidden_check(c, CHILD(n, i+1), STR(CHILD(n, i+1))))
                    return NULL;
                vararg = NEW_IDENTIFIER(CHILD(n, i+1));
                if (!vararg)
                    return NULL;
                i += 2; /* the star and the name */
                if (i < NCH(n) && TYPE(CHILD(n, i)) == COMMA)
                    i += 1; /* the comma, if present */
                break;
            case DOUBLESTAR:
                if (!forbidden_check(c, CHILD(n, i+1), STR(CHILD(n, i+1))))
                    return NULL;
                kwarg = NEW_IDENTIFIER(CHILD(n, i+1));
                if (!kwarg)
                    return NULL;
                i += 2; /* the double star and the name */
                if (i < NCH(n) && TYPE(CHILD(n, i)) == COMMA)
                    i += 1; /* the comma, if present */
                break;
            case TYPE_COMMENT:
                assert(l < k + !!vararg + !!kwarg);

                if (!type_comments) {
                    /* lazily allocate the type_comments seq for perf reasons */
                    type_comments = asdl_seq_new(n_all_args, c->c_arena);
                    if (!type_comments)
                        return NULL;
                }

                while (l < k + !!vararg + !!kwarg - 1) {
                    asdl_seq_SET(type_comments, l++, NULL);
                }

                asdl_seq_SET(type_comments, l++, NEW_TYPE_COMMENT(ch));
                i += 1;
                break;
            default:
                PyErr_Format(PyExc_SystemError,
                             "unexpected node in varargslist: %d @ %d",
                             TYPE(ch), i);
                return NULL;
        }
    }

    if (type_comments) {
        while (l < n_all_args) {
            asdl_seq_SET(type_comments, l++, NULL);
        }
    }

    return arguments(args, vararg, kwarg, defaults, type_comments, c->c_arena);
}