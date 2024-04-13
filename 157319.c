void imap_error (const char *where, const char *msg)
{
  mutt_error ("%s [%s]\n", where, msg);
  mutt_sleep (2);
}