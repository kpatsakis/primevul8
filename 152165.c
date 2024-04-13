MagickExport const void *GetVirtualMetacontent(const Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  const void
    *magick_restrict metacontent;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  metacontent=cache_info->methods.get_virtual_metacontent_from_handler(image);
  if (metacontent != (void *) NULL)
    return(metacontent);
  assert(id < (int) cache_info->number_threads);
  metacontent=GetVirtualMetacontentFromNexus(cache_info,
    cache_info->nexus_info[id]);
  return(metacontent);
}