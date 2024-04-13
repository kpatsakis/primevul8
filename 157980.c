num_stmts(const node *n)
{
    int i, l;
    node *ch;

    switch (TYPE(n)) {
        case single_input:
            if (TYPE(CHILD(n, 0)) == NEWLINE)
                return 0;
            else
                return num_stmts(CHILD(n, 0));
        case file_input:
            l = 0;
            for (i = 0; i < NCH(n); i++) {
                ch = CHILD(n, i);
                if (TYPE(ch) == stmt)
                    l += num_stmts(ch);
            }
            return l;
        case stmt:
            return num_stmts(CHILD(n, 0));
        case compound_stmt:
            return 1;
        case simple_stmt:
            return NCH(n) / 2; /* Divide by 2 to remove count of semi-colons */
        case suite:
        case func_body_suite:
            /* func_body_suite: simple_stmt | NEWLINE [TYPE_COMMENT NEWLINE] INDENT stmt+ DEDENT */
            /* suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT */
            if (NCH(n) == 1)
                return num_stmts(CHILD(n, 0));
            else {
                i = 2;
                l = 0;
                if (TYPE(CHILD(n, 1)) == TYPE_COMMENT)
                    i += 2;
                for (; i < (NCH(n) - 1); i++)
                    l += num_stmts(CHILD(n, i));
                return l;
            }
        default: {
            char buf[128];

            sprintf(buf, "Non-statement found: %d %d",
                    TYPE(n), NCH(n));
            Py_FatalError(buf);
        }
    }
    Py_UNREACHABLE();
}