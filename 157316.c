static int imap_hcache_namer (const char* path, char* dest, size_t dlen)
{
  return snprintf (dest, dlen, "%s.hcache", path);
}