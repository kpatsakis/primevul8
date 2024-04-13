_pattern_parse(const char * input,
               const char **out_pattern,
               gboolean *   out_is_inverted,
               gboolean *   out_is_mandatory)
{
    gboolean is_inverted  = FALSE;
    gboolean is_mandatory = FALSE;

    if (input[0] == '&') {
        input++;
        is_mandatory = TRUE;
        if (input[0] == '!') {
            input++;
            is_inverted = TRUE;
        }
        goto out;
    }

    if (input[0] == '|') {
        input++;
        if (input[0] == '!') {
            input++;
            is_inverted = TRUE;
        }
        goto out;
    }

    if (input[0] == '!') {
        input++;
        is_inverted  = TRUE;
        is_mandatory = TRUE;
        goto out;
    }

out:
    if (input[0] == '\\')
        input++;

    *out_pattern      = input;
    *out_is_inverted  = is_inverted;
    *out_is_mandatory = is_mandatory;
}