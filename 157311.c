char* imap_get_qualifier (char* buf)
{
  char *s = buf;

  /* skip tag */
  s = imap_next_word (s);
  /* skip OK/NO/BAD response */
  s = imap_next_word (s);

  return s;
}