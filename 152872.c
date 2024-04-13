buf_ungetc(int c, struct tok_state *tok) {
    tok->str--;
    assert(Py_CHARMASK(*tok->str) == c);        /* tok->cur may point to read-only segment */
}