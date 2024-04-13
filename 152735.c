get_normal_name(const char *s)  /* for utf-8 and latin-1 */
{
    char buf[13];
    int i;
    for (i = 0; i < 12; i++) {
        int c = s[i];
        if (c == '\0')
            break;
        else if (c == '_')
            buf[i] = '-';
        else
            buf[i] = tolower(c);
    }
    buf[i] = '\0';
    if (strcmp(buf, "utf-8") == 0 ||
        strncmp(buf, "utf-8-", 6) == 0)
        return "utf-8";
    else if (strcmp(buf, "latin-1") == 0 ||
             strcmp(buf, "iso-8859-1") == 0 ||
             strcmp(buf, "iso-latin-1") == 0 ||
             strncmp(buf, "latin-1-", 8) == 0 ||
             strncmp(buf, "iso-8859-1-", 11) == 0 ||
             strncmp(buf, "iso-latin-1-", 12) == 0)
        return "iso-8859-1";
    else
        return s;
}