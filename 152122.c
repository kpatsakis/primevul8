MagickPrivate MagickSizeType GetPixelCacheNexusExtent(const Cache cache,
  NexusInfo *magick_restrict nexus_info)
{
  CacheInfo
    *magick_restrict cache_info;

  MagickSizeType
    extent;

  assert(cache != NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickCoreSignature);
  extent=(MagickSizeType) nexus_info->region.width*nexus_info->region.height;
  if (extent == 0)
    return((MagickSizeType) cache_info->columns*cache_info->rows);
  return(extent);
}