setup_header(const uschar *hstring)
{
const uschar *p, *q;
int hlen = Ustrlen(hstring);

/* Ignore any leading newlines */
while (*hstring == '\n') hstring++, hlen--;

/* An empty string does nothing; ensure exactly one final newline. */
if (hlen <= 0) return;
if (hstring[--hlen] != '\n')		/* no newline */
  q = string_sprintf("%s\n", hstring);
else if (hstring[hlen-1] == '\n')	/* double newline */
  {
  uschar * s = string_copy(hstring);
  while(s[--hlen] == '\n')
    s[hlen+1] = '\0';
  q = s;
  }
else
  q = hstring;

/* Loop for multiple header lines, taking care about continuations */

for (p = q; *p; p = q)
  {
  const uschar *s;
  uschar * hdr;
  int newtype = htype_add_bot;
  header_line **hptr = &acl_added_headers;

  /* Find next header line within the string */

  for (;;)
    {
    q = Ustrchr(q, '\n');		/* we know there was a newline */
    if (*++q != ' ' && *q != '\t') break;
    }

  /* If the line starts with a colon, interpret the instruction for where to
  add it. This temporarily sets up a new type. */

  if (*p == ':')
    {
    if (strncmpic(p, US":after_received:", 16) == 0)
      {
      newtype = htype_add_rec;
      p += 16;
      }
    else if (strncmpic(p, US":at_start_rfc:", 14) == 0)
      {
      newtype = htype_add_rfc;
      p += 14;
      }
    else if (strncmpic(p, US":at_start:", 10) == 0)
      {
      newtype = htype_add_top;
      p += 10;
      }
    else if (strncmpic(p, US":at_end:", 8) == 0)
      {
      newtype = htype_add_bot;
      p += 8;
      }
    while (*p == ' ' || *p == '\t') p++;
    }

  /* See if this line starts with a header name, and if not, add X-ACL-Warn:
  to the front of it. */

  for (s = p; s < q - 1; s++)
    if (*s == ':' || !isgraph(*s)) break;

  hdr = string_sprintf("%s%.*s", *s == ':' ? "" : "X-ACL-Warn: ", (int) (q - p), p);
  hlen = Ustrlen(hdr);

  /* See if this line has already been added */

  while (*hptr)
    {
    if (Ustrncmp((*hptr)->text, hdr, hlen) == 0) break;
    hptr = &(*hptr)->next;
    }

  /* Add if not previously present */

  if (!*hptr)
    {
    header_line *h = store_get(sizeof(header_line));
    h->text = hdr;
    h->next = NULL;
    h->type = newtype;
    h->slen = hlen;
    *hptr = h;
    hptr = &h->next;
    }
  }
}