MagickExport void *GetAuthenticMetacontent(const Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->methods.get_authentic_metacontent_from_handler !=
      (GetAuthenticMetacontentFromHandler) NULL)
    {
      void
        *metacontent;

      metacontent=cache_info->methods.
        get_authentic_metacontent_from_handler(image);
      return(metacontent);
    }
  assert(id < (int) cache_info->number_threads);
  return(cache_info->nexus_info[id]->metacontent);
}