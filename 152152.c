MagickPrivate const void *GetVirtualMetacontentFromNexus(const Cache cache,
  NexusInfo *magick_restrict nexus_info)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->storage_class == UndefinedClass)
    return((void *) NULL);
  return(nexus_info->metacontent);
}