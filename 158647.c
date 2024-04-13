contains_header(uschar *hdr, uschar *hstring)
{
int len = Ustrlen(hdr);
uschar *p = hstring;
while (*p != 0)
  {
  if (strncmpic(p, hdr, len) == 0)
    {
    p += len;
    while (*p == ' ' || *p == '\t') p++;
    if (*p == ':') return TRUE;
    }
  while (*p != 0 && *p != '\n') p++;
  if (*p == '\n') p++;
  }
return FALSE;
}