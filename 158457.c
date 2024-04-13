one_pattern_match(uschar *name, int slen, BOOL has_addresses, uschar *pattern)
{
BOOL yield = FALSE;
header_line *h;
const pcre *re = NULL;

/* If the pattern is a regex, compile it. Bomb out if compiling fails; these
patterns are all constructed internally and should be valid. */

if (*pattern == '^') re = regex_must_compile(pattern, TRUE, FALSE);

/* Scan for the required header(s) and scan each one */

for (h = header_list; !yield && h != NULL; h = h->next)
  {
  if (h->type == htype_old || slen > h->slen ||
      strncmpic(name, h->text, slen) != 0)
    continue;

  /* If the header is a list of addresses, extract each one in turn, and scan
  it. A non-regex scan must be an exact match for the address. */

  if (has_addresses)
    {
    uschar *s = h->text + slen;

    while (!yield && *s != 0)
      {
      uschar *error, *next;
      uschar *e = parse_find_address_end(s, FALSE);
      int terminator = *e;
      int start, end, domain;

      /* Temporarily terminate the string at the address end while extracting
      the operative address within. */

      *e = 0;
      next = parse_extract_address(s, &error, &start, &end, &domain, FALSE);
      *e = terminator;

      /* Move on, ready for the next address */

      s = e;
      if (*s == ',') s++;

      /* If there is some kind of syntax error, just give up on this header
      line. */

      if (next == NULL) break;

      /* Otherwise, test for the pattern; a non-regex must be an exact match */

      yield = (re == NULL)?
        (strcmpic(next, pattern) == 0)
        :
        (pcre_exec(re, NULL, CS next, Ustrlen(next), 0, PCRE_EOPT, NULL, 0)
          >= 0);
      }
    }

  /* For headers that are not lists of addresses, scan the entire header line,
  and just require "contains" for non-regex patterns. */

  else
    {
    yield = (re == NULL)?
      (strstric(h->text, pattern, FALSE) != NULL)
      :
      (pcre_exec(re, NULL, CS h->text, h->slen, 0, PCRE_EOPT, NULL, 0) >= 0);
    }
  }

return yield;
}