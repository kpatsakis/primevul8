tok_decimal_tail(struct tok_state *tok)
{
    int c;

    while (1) {
        do {
            c = tok_nextc(tok);
        } while (isdigit(c));
        if (c != '_') {
            break;
        }
        c = tok_nextc(tok);
        if (!isdigit(c)) {
            tok->done = E_TOKEN;
            tok_backup(tok, c);
            return 0;
        }
    }
    return c;
}