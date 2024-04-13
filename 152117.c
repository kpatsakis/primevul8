MagickPrivate VirtualPixelMethod SetPixelCacheVirtualMethod(Image *image,
  const VirtualPixelMethod virtual_pixel_method,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  VirtualPixelMethod
    method;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  method=cache_info->virtual_pixel_method;
  cache_info->virtual_pixel_method=virtual_pixel_method;
  if ((image->columns != 0) && (image->rows != 0))
    switch (virtual_pixel_method)
    {
      case BackgroundVirtualPixelMethod:
      {
        if ((image->background_color.alpha_trait != UndefinedPixelTrait) &&
            (image->alpha_trait == UndefinedPixelTrait))
          (void) SetCacheAlphaChannel(image,OpaqueAlpha,exception);
        if ((IsPixelInfoGray(&image->background_color) == MagickFalse) &&
            (IsGrayColorspace(image->colorspace) != MagickFalse))
          (void) SetImageColorspace(image,sRGBColorspace,exception);
        break;
      }
      case TransparentVirtualPixelMethod:
      {
        if (image->alpha_trait == UndefinedPixelTrait)
          (void) SetCacheAlphaChannel(image,OpaqueAlpha,exception);
        break;
      }
      default:
        break;
    }
  return(method);
}