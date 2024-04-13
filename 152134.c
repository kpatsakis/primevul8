MagickExport MagickBooleanType GetOneAuthenticPixel(Image *image,
  const ssize_t x,const ssize_t y,Quantum *pixel,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  register Quantum
    *magick_restrict q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  (void) memset(pixel,0,MaxPixelChannels*sizeof(*pixel));
  if (cache_info->methods.get_one_authentic_pixel_from_handler !=
       (GetOneAuthenticPixelFromHandler) NULL)
    return(cache_info->methods.get_one_authentic_pixel_from_handler(image,x,y,
      pixel,exception));
  q=GetAuthenticPixelsCache(image,x,y,1UL,1UL,exception);
  return(CopyPixel(image,q,pixel));
}