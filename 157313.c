void imap_unmunge_mbox_name (IMAP_DATA *idata, char *s)
{
  char *buf;

  imap_unquote_string(s);

  buf = safe_strdup (s);
  if (buf)
  {
    imap_utf_decode (idata, &buf);
    strncpy (s, buf, strlen (s));
  }

  FREE (&buf);
}