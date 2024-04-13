string_printing2(const uschar *s, BOOL allow_tab)
{
int nonprintcount = 0;
int length = 0;
const uschar *t = s;
uschar *ss, *tt;

while (*t != 0)
  {
  int c = *t++;
  if (!mac_isprint(c) || (!allow_tab && c == '\t')) nonprintcount++;
  length++;
  }

if (nonprintcount == 0) return s;

/* Get a new block of store guaranteed big enough to hold the
expanded string. */

ss = store_get(length + nonprintcount * 3 + 1);

/* Copy everything, escaping non printers. */

t = s;
tt = ss;

while (*t != 0)
  {
  int c = *t;
  if (mac_isprint(c) && (allow_tab || c != '\t')) *tt++ = *t++; else
    {
    *tt++ = '\\';
    switch (*t)
      {
      case '\n': *tt++ = 'n'; break;
      case '\r': *tt++ = 'r'; break;
      case '\b': *tt++ = 'b'; break;
      case '\v': *tt++ = 'v'; break;
      case '\f': *tt++ = 'f'; break;
      case '\t': *tt++ = 't'; break;
      default: sprintf(CS tt, "%03o", *t); tt += 3; break;
      }
    t++;
    }
  }
*tt = 0;
return ss;
}