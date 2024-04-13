transport_headers_send(transport_ctx * tctx,
  BOOL (*sendfn)(transport_ctx * tctx, uschar * s, int len))
{
header_line *h;
const uschar *list;
transport_instance * tblock = tctx ? tctx->tblock : NULL;
address_item * addr = tctx ? tctx->addr : NULL;

/* Then the message's headers. Don't write any that are flagged as "old";
that means they were rewritten, or are a record of envelope rewriting, or
were removed (e.g. Bcc). If remove_headers is not null, skip any headers that
match any entries therein.  It is a colon-sep list; expand the items
separately and squash any empty ones.
Then check addr->prop.remove_headers too, provided that addr is not NULL. */

for (h = header_list; h; h = h->next) if (h->type != htype_old)
  {
  int i;
  BOOL include_header = TRUE;

  list = tblock ? tblock->remove_headers : NULL;
  for (i = 0; i < 2; i++)    /* For remove_headers && addr->prop.remove_headers */
    {
    if (list)
      {
      int sep = ':';         /* This is specified as a colon-separated list */
      uschar *s, *ss;
      while ((s = string_nextinlist(&list, &sep, NULL, 0)))
	{
	int len;

	if (i == 0)
	  if (!(s = expand_string(s)) && !f.expand_string_forcedfail)
	    {
	    errno = ERRNO_CHHEADER_FAIL;
	    return FALSE;
	    }
	len = s ? Ustrlen(s) : 0;
	if (strncmpic(h->text, s, len) != 0) continue;
	ss = h->text + len;
	while (*ss == ' ' || *ss == '\t') ss++;
	if (*ss == ':') break;
	}
      if (s) { include_header = FALSE; break; }
      }
    if (addr) list = addr->prop.remove_headers;
    }

  /* If this header is to be output, try to rewrite it if there are rewriting
  rules. */

  if (include_header)
    {
    if (tblock && tblock->rewrite_rules)
      {
      void *reset_point = store_get(0);
      header_line *hh;

      if ((hh = rewrite_header(h, NULL, NULL, tblock->rewrite_rules,
		  tblock->rewrite_existflags, FALSE)))
	{
	if (!sendfn(tctx, hh->text, hh->slen)) return FALSE;
	store_reset(reset_point);
	continue;     /* With the next header line */
	}
      }

    /* Either no rewriting rules, or it didn't get rewritten */

    if (!sendfn(tctx, h->text, h->slen)) return FALSE;
    }

  /* Header removed */

  else
    DEBUG(D_transport) debug_printf("removed header line:\n%s---\n", h->text);
  }

/* Add on any address-specific headers. If there are multiple addresses,
they will all have the same headers in order to be batched. The headers
are chained in reverse order of adding (so several addresses from the
same alias might share some of them) but we want to output them in the
opposite order. This is a bit tedious, but there shouldn't be very many
of them. We just walk the list twice, reversing the pointers each time,
but on the second time, write out the items.

Headers added to an address by a router are guaranteed to end with a newline.
*/

if (addr)
  {
  int i;
  header_line *hprev = addr->prop.extra_headers;
  header_line *hnext;
  for (i = 0; i < 2; i++)
    for (h = hprev, hprev = NULL; h; h = hnext)
      {
      hnext = h->next;
      h->next = hprev;
      hprev = h;
      if (i == 1)
	{
	if (!sendfn(tctx, h->text, h->slen)) return FALSE;
	DEBUG(D_transport)
	  debug_printf("added header line(s):\n%s---\n", h->text);
	}
      }
  }

/* If a string containing additional headers exists it is a newline-sep
list.  Expand each item and write out the result.  This is done last so that
if it (deliberately or accidentally) isn't in header format, it won't mess
up any other headers. An empty string or a forced expansion failure are
noops. An added header string from a transport may not end with a newline;
add one if it does not. */

if (tblock && (list = CUS tblock->add_headers))
  {
  int sep = '\n';
  uschar * s;

  while ((s = string_nextinlist(&list, &sep, NULL, 0)))
    if ((s = expand_string(s)))
      {
      int len = Ustrlen(s);
      if (len > 0)
	{
	if (!sendfn(tctx, s, len)) return FALSE;
	if (s[len-1] != '\n' && !sendfn(tctx, US"\n", 1))
	  return FALSE;
	DEBUG(D_transport)
	  {
	  debug_printf("added header line:\n%s", s);
	  if (s[len-1] != '\n') debug_printf("\n");
	  debug_printf("---\n");
	  }
	}
      }
    else if (!f.expand_string_forcedfail)
      { errno = ERRNO_CHHEADER_FAIL; return FALSE; }
  }

/* Separate headers from body with a blank line */

return sendfn(tctx, US"\n", 1);
}