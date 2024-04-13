in_list(const char *list, const char *item)
{
    const char *p;
    int len = strlen(item);

    if (list == NULL)
        return FALSE;
    for (p = strstr(list, item); p != NULL; p = strstr(p + 1, item)) {
        if ((p == list || isspace((unsigned char)p[-1]) || p[-1] == ',') &&
            (p[len] == '\0' || isspace((unsigned char)p[len]) ||
             p[len] == ','))
                return TRUE;
    }
    return FALSE;
}