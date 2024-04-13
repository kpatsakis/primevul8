header_add_backend(BOOL after, uschar *name, BOOL topnot, int type,
  const char *format, va_list ap)
{
header_line *h, *new;
header_line **hptr;

uschar *p, *q;
uschar buffer[HEADER_ADD_BUFFER_SIZE];
gstring gs = { .size = HEADER_ADD_BUFFER_SIZE, .ptr = 0, .s = buffer };

if (!header_last) return;

if (!string_vformat(&gs, FALSE, format, ap))
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "string too long in header_add: "
    "%.100s ...", string_from_gstring(&gs));
string_from_gstring(&gs);

/* Find where to insert this header */

if (!name)
  {
  if (after)
    {
    hptr = &(header_last->next);
    h = NULL;
    }
  else
    {
    hptr = &header_list;

    /* header_list->text can be NULL if we get here between when the new
    received header is allocated and when it is actually filled in. We want
    that header to be first, so skip it for now. */

    if (!header_list->text)
      hptr = &header_list->next;
    h = *hptr;
    }
  }

else
  {
  int len = Ustrlen(name);

  /* Find the first non-deleted header with the correct name. */

  for (hptr = &header_list; (h = *hptr); hptr = &h->next)
    if (header_testname(h, name, len, TRUE))
      break;

  /* Handle the case where no header is found. To insert at the bottom, nothing
  needs to be done. */

  if (!h)
    {
    if (topnot)
      {
      hptr = &header_list;
      h = header_list;
      }
    }

  /* Handle the case where a header is found. Check for more if "after" is
  true. In this case, we want to include deleted headers in the block. */

  else if (after)
    for (;;)
      {
      if (!h->next || !header_testname(h, name, len, FALSE)) break;
      hptr = &(h->next);
      h = h->next;
      }
  }

/* Loop for multiple header lines, taking care about continuations. At this
point, we have hptr pointing to the link field that will point to the new
header, and h containing the following header, or NULL. */

for (p = q = buffer; *p != 0; )
  {
  for (;;)
    {
    q = Ustrchr(q, '\n');
    if (!q) q = p + Ustrlen(p);
    if (*(++q) != ' ' && *q != '\t') break;
    }

  new = store_get(sizeof(header_line));
  new->text = string_copyn(p, q - p);
  new->slen = q - p;
  new->type = type;
  new->next = h;

  *hptr = new;
  hptr = &(new->next);

  if (!h) header_last = new;
  p = q;
  }
}