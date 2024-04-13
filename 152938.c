buf_setreadl(struct tok_state *tok, const char* enc) {
    tok->enc = enc;
    return 1;
}