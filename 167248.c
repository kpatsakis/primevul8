static char *unescape_and_return_next_file(char * const str) {
    char *pnt = str;
    signed char seen_backslash = 0;

    while (*pnt != 0) {
        if (seen_backslash == 0) {
            if (*pnt == '\\') {
                seen_backslash = 1;
            } else if (*pnt == ' ') {
                *pnt++ = 0;
                if (*pnt != 0) {
                    return pnt;
                }
                break;
            }
            pnt++;
        } else {
            seen_backslash = 0;
            if (*pnt == ' ' || *pnt == '\\' || *pnt == '{' || *pnt == '}') {
                memmove(pnt - 1, pnt, strlen(pnt) + (size_t) 1U);
            }
        }
    }
    return NULL;
}