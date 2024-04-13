classify(parser_state *ps, int type, const char *str)
{
    grammar *g = ps->p_grammar;
    int n = g->g_ll.ll_nlabels;

    if (type == NAME) {
        label *l = g->g_ll.ll_label;
        int i;
        for (i = n; i > 0; i--, l++) {
            if (l->lb_type != NAME || l->lb_str == NULL ||
                l->lb_str[0] != str[0] ||
                strcmp(l->lb_str, str) != 0)
                continue;
#ifdef PY_PARSER_REQUIRES_FUTURE_KEYWORD
#if 0
            /* Leaving this in as an example */
            if (!(ps->p_flags & CO_FUTURE_WITH_STATEMENT)) {
                if (str[0] == 'w' && strcmp(str, "with") == 0)
                    break; /* not a keyword yet */
                else if (str[0] == 'a' && strcmp(str, "as") == 0)
                    break; /* not a keyword yet */
            }
#endif
#endif
            D(printf("It's a keyword\n"));
            return n - i;
        }
    }

    {
        label *l = g->g_ll.ll_label;
        int i;
        for (i = n; i > 0; i--, l++) {
            if (l->lb_type == type && l->lb_str == NULL) {
                D(printf("It's a token we know\n"));
                return n - i;
            }
        }
    }

    D(printf("Illegal token\n"));
    return -1;
}