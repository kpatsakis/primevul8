decoding_fgets(char *s, int size, struct tok_state *tok)
{
    return fgets(s, size, tok->fp);
}