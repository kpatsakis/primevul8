MagickExport void DestroyImagePixels(Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->methods.destroy_pixel_handler != (DestroyPixelHandler) NULL)
    {
      cache_info->methods.destroy_pixel_handler(image);
      return;
    }
  image->cache=DestroyPixelCache(image->cache);
}