MagickPrivate MagickBooleanType SyncImagePixelCache(Image *image,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (Image *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  cache_info=(CacheInfo *) GetImagePixelCache(image,MagickTrue,exception);
  return(cache_info == (CacheInfo *) NULL ? MagickFalse : MagickTrue);
}