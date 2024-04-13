PARAMETER *rfc2231_encode_string (const char *attribute, char *value)
{
  int encode = 0, add_quotes = 0, free_src_value = 0;
  int split = 0, continuation_number = 0;
  size_t dest_value_len = 0, max_value_len = 0, cur_value_len = 0;
  char *cur, *charset = NULL, *src_value = NULL;
  PARAMETER *result = NULL, *current, **lastp;
  BUFFER *cur_attribute, *cur_value;

  cur_attribute = mutt_buffer_pool_get ();
  cur_value = mutt_buffer_pool_get ();

  /*
   * Perform charset conversion
   */
  for (cur = value; *cur; cur++)
    if (*cur < 0x20 || *cur >= 0x7f)
    {
      encode = 1;
      break;
    }

  if (encode)
  {
    if (Charset && SendCharset)
      charset = mutt_choose_charset (Charset, SendCharset,
                                     value, mutt_strlen (value),
                                     &src_value, NULL);
    if (src_value)
      free_src_value = 1;
    if (!charset)
      charset = safe_strdup (Charset ? Charset : "unknown-8bit");
  }
  if (!src_value)
    src_value = value;

  /*
   * Count the size the resultant value will need in total.
   */
  if (encode)
    dest_value_len = mutt_strlen (charset) + 2;  /* charset'' prefix */

  for (cur = src_value; *cur; cur++)
  {
    dest_value_len++;

    if (encode)
    {
      /* These get converted to %xx so need a total of three chars */
      if (*cur < 0x20 || *cur >= 0x7f ||
          strchr (MimeSpecials, *cur) ||
          strchr ("*'%", *cur))
      {
        dest_value_len += 2;
      }
    }
    else
    {
      /* rfc822_cat() will add outer quotes if it finds MimeSpecials. */
      if (!add_quotes && strchr (MimeSpecials, *cur))
        add_quotes = 1;
      /* rfc822_cat() will add a backslash if it finds '\' or '"'. */
      if (*cur == '\\' || *cur == '"')
        dest_value_len++;
    }
  }

  /*
   * Determine if need to split into parameter value continuations
   */
  max_value_len =
    78                      -    /* rfc suggested line length */
    1                       -    /* Leading tab on continuation line */
    mutt_strlen (attribute) -    /* attribute */
    (encode ? 1 : 0)        -    /* '*' encoding marker */
    1                       -    /* '=' */
    (add_quotes ? 2 : 0)    -    /* "...." */
    1;                           /* ';' */

  if (max_value_len < 30)
    max_value_len = 30;

  if (dest_value_len > max_value_len)
  {
    split = 1;
    max_value_len -= 4;          /* '*n' continuation number and extra encoding
                                  * space to keep loop below simpler */
  }

  /*
   * Generate list of parameter continuations.
   */
  lastp = &result;
  cur = src_value;
  if (encode)
  {
    mutt_buffer_printf (cur_value, "%s''", charset);
    cur_value_len = mutt_buffer_len (cur_value);
  }

  while (*cur)
  {
    *lastp = current = mutt_new_parameter ();
    lastp = &current->next;
    mutt_buffer_strcpy (cur_attribute, attribute);
    if (split)
      mutt_buffer_add_printf (cur_attribute, "*%d", continuation_number++);
    if (encode)
      mutt_buffer_addch (cur_attribute, '*');

    while (*cur && (!split || cur_value_len < max_value_len))
    {
      if (encode)
      {
        if (*cur < 0x20 || *cur >= 0x7f ||
            strchr (MimeSpecials, *cur) ||
            strchr ("*'%", *cur))
        {
          mutt_buffer_add_printf (cur_value, "%%%02X", (unsigned char)*cur);
          cur_value_len += 3;
        }
        else
        {
          mutt_buffer_addch (cur_value, *cur);
          cur_value_len++;
        }
      }
      else
      {
        mutt_buffer_addch (cur_value, *cur);
        cur_value_len++;
        if (*cur == '\\' || *cur == '"')
          cur_value_len++;
      }

      cur++;
    }

    current->attribute = safe_strdup (mutt_b2s (cur_attribute));
    current->value = safe_strdup (mutt_b2s (cur_value));

    mutt_buffer_clear (cur_value);
    cur_value_len = 0;
  }

  mutt_buffer_pool_release (&cur_attribute);
  mutt_buffer_pool_release (&cur_value);

  FREE (&charset);
  if (free_src_value)
    FREE (&src_value);

  return result;
}