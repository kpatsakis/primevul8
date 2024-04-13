static void rfc2231_join_continuations (PARAMETER **head,
					struct rfc2231_parameter *par)
{
  struct rfc2231_parameter *q;

  char attribute[STRING];
  char charset[STRING];
  char *value = NULL;
  char *valp;
  int encoded;

  size_t l, vl;

  while (par)
  {
    value = NULL; l = 0;

    strfcpy (attribute, par->attribute, sizeof (attribute));

    if ((encoded = par->encoded))
      valp = rfc2231_get_charset (par->value, charset, sizeof (charset));
    else
      valp = par->value;

    do
    {
      if (encoded && par->encoded)
	rfc2231_decode_one (par->value, valp);

      vl = strlen (par->value);

      safe_realloc (&value, l + vl + 1);
      strcpy (value + l, par->value);	/* __STRCPY_CHECKED__ */
      l += vl;

      q = par->next;
      rfc2231_free_parameter (&par);
      if ((par = q))
	valp = par->value;
    } while (par && !strcmp (par->attribute, attribute));

    if (value)
    {
      if (encoded)
	mutt_convert_string (&value, charset, Charset, MUTT_ICONV_HOOK_FROM);
      *head = mutt_new_parameter ();
      (*head)->attribute = safe_strdup (attribute);
      (*head)->value = value;
      head = &(*head)->next;
    }
  }
}