void imap_free_idata (IMAP_DATA** idata)
{
  if (!idata)
    return;

  FREE (&(*idata)->capstr);
  mutt_free_list (&(*idata)->flags);
  imap_mboxcache_free (*idata);
  mutt_buffer_free(&(*idata)->cmdbuf);
  FREE (&(*idata)->buf);
  mutt_bcache_close (&(*idata)->bcache);
  FREE (&(*idata)->cmds);
  FREE (idata);		/* __FREE_CHECKED__ */
}