xml_check_first_element_from_data (const guint8 * data, guint length,
    const gchar * element, guint elen, gboolean strict)
{
  gboolean got_xmldec;
  guint pos = 0;

  g_return_val_if_fail (data != NULL, FALSE);

  if (length <= 5)
    return FALSE;

  /* look for the XMLDec
   * see XML spec 2.8, Prolog and Document Type Declaration
   * http://www.w3.org/TR/2004/REC-xml-20040204/#sec-prolog-dtd */
  got_xmldec = (memcmp (data, "<?xml", 5) == 0);

  if (strict && !got_xmldec)
    return FALSE;

  /* skip XMLDec in any case if we've got one */
  if (got_xmldec) {
    pos += 5;
    data += 5;
  }

  /* look for the first element, it has to be the requested element. Bail
   * out if it is not within the first 4kB. */
  while (pos < MIN (4096, length)) {
    while (*data != '<' && pos < MIN (4096, length)) {
      XML_INC_BUFFER_DATA;
    }

    XML_INC_BUFFER_DATA;
    if (!g_ascii_isalpha (*data)) {
      /* if not alphabetic, it's a PI or an element / attribute declaration
       * like <?xxx or <!xxx */
      XML_INC_BUFFER_DATA;
      continue;
    }

    /* the first normal element, check if it's the one asked for */
    if (pos + elen + 1 >= length)
      return FALSE;
    return (element && strncmp ((const char *) data, element, elen) == 0);
  }

  return FALSE;
}