void imap_qualify_path (char *dest, size_t len, IMAP_MBOX *mx, char* path)
{
  ciss_url_t url;

  mutt_account_tourl (&mx->account, &url);
  url.path = path;

  url_ciss_tostring (&url, dest, len, 0);
}