match(Char *name, Char *pat, Char *patend, int recur)
{
    int ok, negate_range;
    Char c, k;

    if (recur-- == 0) {
        return GLOB_NOSPACE;
    }
    while (pat < patend) {
        c = *pat++;
        switch (c & M_MASK) {
        case M_ALL:
            while (pat < patend && (*pat & M_MASK) == M_ALL) {
                pat++;  /* eat consecutive '*' */
            }
            if (pat == patend) {
                return 1;
            }
            do {
                if (match(name, pat, patend, recur)) {
                    return 1;
                }
            } while (*name++ != EOS);
            return 0;
        case M_ONE:
            if (*name++ == EOS) {
                return 0;
            }
            break;
        case M_SET:
            ok = 0;
            if ((k = *name++) == EOS) {
                return 0;
            }
            if ((negate_range = ((*pat & M_MASK) == M_NOT)) != EOS) {
                ++pat;
            }
            while (((c = *pat++) & M_MASK) != M_END) {
                if ((*pat & M_MASK) == M_RNG) {
                    if (c <= k && k <= pat[1]) {
                        ok = 1;
                    }
                    pat += 2;
                } else if (c == k) {
                    ok = 1;
                }
            }
            if (ok == negate_range) {
                return 0;
            }
            break;
        default:
            if (*name++ != c) {
                return 0;
            }
            break;
        }
    }
    return *name == EOS;
}