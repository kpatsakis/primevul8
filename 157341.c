int imap_continue (const char* msg, const char* resp)
{
  imap_error (msg, resp);
  return mutt_yesorno (_("Continue?"), 0);
}