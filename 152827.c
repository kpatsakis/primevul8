static void fp_ungetc(int c, struct tok_state *tok) {
    ungetc(c, tok->fp);
}