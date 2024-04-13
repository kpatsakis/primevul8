header_testname_incomplete(header_line *h, const uschar *name,
    int len, BOOL notdel)
{
if (h->type == '*' && notdel) return FALSE;
if (h->text == NULL || strncmpic(h->text, name, len) != 0) return FALSE;
return TRUE;
}