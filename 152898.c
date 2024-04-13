decode_str(const char *str, int exec_input, struct tok_state *tok)
{
    return new_string(str, strlen(str), tok);
}