indenterror(struct tok_state *tok)
{
    tok->done = E_TABSPACE;
    tok->cur = tok->inp;
    return ERRORTOKEN;
}