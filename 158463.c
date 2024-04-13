header_testname(header_line *h, const uschar *name, int len, BOOL notdel)
{
uschar *tt;
if (h->type == '*' && notdel) return FALSE;
if (h->text == NULL || strncmpic(h->text, name, len) != 0) return FALSE;
tt = h->text + len;
while (*tt == ' ' || *tt == '\t') tt++;
return *tt == ':';
}