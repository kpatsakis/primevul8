buf_getc(struct tok_state *tok) {
    return Py_CHARMASK(*tok->str++);
}