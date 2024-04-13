header_checkname(header_line *h, BOOL is_resent)
{
uschar *text = h->text;
header_name *bot = header_names;
header_name *top = header_names + header_names_size;

if (is_resent) text += 7;

while (bot < top)
  {
  header_name *mid = bot + (top - bot)/2;
  int c = strncmpic(text, mid->name, mid->len);

  if (c == 0)
    {
    uschar *s = text + mid->len;
    while (isspace(*s)) s++;
    if (*s == ':')
      return (!is_resent || mid->allow_resent)? mid->htype : htype_other;
    c = 1;
    }

  if (c > 0) bot = mid + 1; else top = mid;
  }

return htype_other;
}