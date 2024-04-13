int imap_get_literal_count(const char *buf, unsigned int *bytes)
{
  char *pc;
  char *pn;

  if (!buf || !(pc = strchr (buf, '{')))
    return -1;

  pc++;
  pn = pc;
  while (isdigit ((unsigned char) *pc))
    pc++;
  *pc = 0;
  if (mutt_atoui (pn, bytes) < 0)
    return -1;

  return 0;
}