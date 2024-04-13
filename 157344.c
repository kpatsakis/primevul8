int imap_hcache_put (IMAP_DATA* idata, HEADER* h)
{
  char key[16];

  if (!idata->hcache)
    return -1;

  sprintf (key, "/%u", HEADER_DATA (h)->uid);
  return mutt_hcache_store (idata->hcache, key, h, idata->uid_validity,
                            imap_hcache_keylen, 0);
}