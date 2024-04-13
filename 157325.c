int imap_hcache_del (IMAP_DATA* idata, unsigned int uid)
{
  char key[16];

  if (!idata->hcache)
    return -1;

  sprintf (key, "/%u", uid);
  return mutt_hcache_delete (idata->hcache, key, imap_hcache_keylen);
}