globexp2(const Char *ptr, const Char *pattern, glob_t *pglob,
         struct glob_lim *limitp, int recursion)
{
    int         i, rv;
    Char       *lm, *ls;
    const Char *pe, *pm, *pl;
    Char        patbuf[PATH_MAX];

    /* copy part up to the brace */
    for (lm = patbuf, pm = pattern; pm != ptr; *lm++ = *pm++)
        ;
    *lm = EOS;
    ls = lm;

    /* Find the balanced brace */
    for (i = 0, pe = ++ptr; *pe; pe++) {
        if (*pe == LBRACKET) {
            /* Ignore everything between [] */
            for (pm = pe++; *pe != RBRACKET && *pe != EOS; pe++)
                ;
            if (*pe == EOS) {
                /*
                 * We could not find a matching RBRACKET.
                 * Ignore and just look for RBRACE
                 */
                pe = pm;
            }
        } else if (*pe == LBRACE) {
            i++;
        } else if (*pe == RBRACE) {
            if (i == 0) {
                break;
            }
            i--;
        }
    }

    /* Non matching braces; just glob the pattern */
    if (i != 0 || *pe == EOS) {
        return glob0(patbuf, pglob, limitp);
    }
    for (i = 0, pl = pm = ptr; pm <= pe; pm++) {
        switch (*pm) {
        case LBRACKET:
            /* Ignore everything between [] */
            for (pl = pm++; *pm != RBRACKET && *pm != EOS; pm++)
                ;
            if (*pm == EOS) {
                /*
                 * We could not find a matching RBRACKET.
                 * Ignore and just look for RBRACE
                 */
                pm = pl;
            }
            break;

        case LBRACE:
            i++;
            break;

        case RBRACE:
            if (i) {
                i--;
                break;
            }
            /* FALLTHROUGH */
        case COMMA:
            if (i && *pm == COMMA) {
                break;
            } else {
                /* Append the current string */
                for (lm = ls; (pl < pm); *lm++ = *pl++)
                    ;

                /*
                 * Append the rest of the pattern after the
                 * closing brace
                 */
                for (pl = pe + 1; (*lm++ = *pl++) != EOS; )
                    ;

                /* Expand the current pattern */
                rv = globexp1(patbuf, pglob, limitp, recursion + 1);
                if (rv && rv != GLOB_NOMATCH) {
                    return rv;
                }
                /* move after the comma, to the next string */
                pl = pm + 1;
            }
            break;

        default:
            break;
        }
    }
    return 0;
}