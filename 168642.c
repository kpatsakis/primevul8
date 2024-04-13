void rfc2231_decode_parameters (PARAMETER **headp)
{
  PARAMETER *head = NULL;
  PARAMETER **last;
  PARAMETER *p, *q;

  struct rfc2231_parameter *conthead = NULL;
  struct rfc2231_parameter *conttmp;

  char *s, *t;
  char charset[STRING];

  int encoded;
  int index;
  short dirty = 0;	/* set to 1 when we may have created
			 * empty parameters.
			 */

  if (!headp) return;

  purge_empty_parameters (headp);

  for (last = &head, p = *headp; p; p = q)
  {
    q = p->next;

    if (!(s = strchr (p->attribute, '*')))
    {

      /*
       * Using RFC 2047 encoding in MIME parameters is explicitly
       * forbidden by that document.  Nevertheless, it's being
       * generated by some software, including certain Lotus Notes to
       * Internet Gateways.  So we actually decode it.
       */

      if (option (OPTRFC2047PARAMS) && p->value && strstr (p->value, "=?"))
	rfc2047_decode (&p->value);
      else if (AssumedCharset && *AssumedCharset)
        convert_nonmime_string (&p->value);

      *last = p;
      last = &p->next;
      p->next = NULL;
    }
    else if (*(s + 1) == '\0')
    {
      *s = '\0';

      s = rfc2231_get_charset (p->value, charset, sizeof (charset));
      rfc2231_decode_one (p->value, s);
      mutt_convert_string (&p->value, charset, Charset, MUTT_ICONV_HOOK_FROM);
      mutt_filter_unprintable (&p->value);

      *last = p;
      last = &p->next;
      p->next = NULL;

      dirty = 1;
    }
    else
    {
      *s = '\0'; s++; /* let s point to the first character of index. */
      for (t = s; *t && isdigit ((unsigned char) *t); t++)
	;
      encoded = (*t == '*');
      *t = '\0';

      /* RFC 2231 says that the index starts at 0 and increments by 1,
         thus an overflow should never occur in a valid message, thus
         the value INT_MAX in case of overflow does not really matter
         (the goal is just to avoid undefined behavior). */
      if (mutt_atoi (s, &index))
        index = INT_MAX;

      conttmp = rfc2231_new_parameter ();
      conttmp->attribute = p->attribute;
      conttmp->value = p->value;
      conttmp->encoded = encoded;
      conttmp->index = index;

      p->attribute = NULL;
      p->value = NULL;
      FREE (&p);

      rfc2231_list_insert (&conthead, conttmp);
    }
  }

  if (conthead)
  {
    rfc2231_join_continuations (last, conthead);
    dirty = 1;
  }

  *headp = head;

  if (dirty)
    purge_empty_parameters (headp);
}