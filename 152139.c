MagickPrivate void GetPixelCacheTileSize(const Image *image,size_t *width,
  size_t *height)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  *width=2048UL/(cache_info->number_channels*sizeof(Quantum));
  if (GetImagePixelCacheType(image) == DiskCache)
    *width=8192UL/(cache_info->number_channels*sizeof(Quantum));
  *height=(*width);
}