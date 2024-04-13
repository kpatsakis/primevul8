void imap_hcache_close (IMAP_DATA* idata)
{
  if (!idata->hcache)
    return;

  mutt_hcache_close (idata->hcache);
  idata->hcache = NULL;
}