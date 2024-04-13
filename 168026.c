_match_result(gboolean has_except,
              gboolean has_not_except,
              gboolean has_match,
              gboolean has_match_except)
{
    if (has_except && !has_not_except) {
        /* a match spec that only consists of a list of except matches is treated specially. */
        nm_assert(!has_match);
        if (has_match_except) {
            /* one of the "except:" matches matched. The result is an explicit
             * negative match. */
            return NM_MATCH_SPEC_NEG_MATCH;
        } else {
            /* none of the "except:" matches matched. The result is a positive match,
             * despite there being no positive match. */
            return NM_MATCH_SPEC_MATCH;
        }
    }

    if (has_match_except)
        return NM_MATCH_SPEC_NEG_MATCH;
    if (has_match)
        return NM_MATCH_SPEC_MATCH;
    return NM_MATCH_SPEC_NO_MATCH;
}