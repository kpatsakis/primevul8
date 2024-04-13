MagickPrivate Quantum *QueueAuthenticPixelCacheNexus(Image *image,
  const ssize_t x,const ssize_t y,const size_t columns,const size_t rows,
  const MagickBooleanType clone,NexusInfo *nexus_info,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  MagickOffsetType
    offset;

  MagickSizeType
    number_pixels;

  Quantum
    *magick_restrict pixels;

  RectangleInfo
    region;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) GetImagePixelCache(image,clone,exception);
  if (cache_info == (Cache) NULL)
    return((Quantum *) NULL);
  assert(cache_info->signature == MagickCoreSignature);
  if ((cache_info->columns == 0) || (cache_info->rows == 0) || (x < 0) ||
      (y < 0) || (x >= (ssize_t) cache_info->columns) ||
      (y >= (ssize_t) cache_info->rows))
    {
      (void) ThrowMagickException(exception,GetMagickModule(),CacheError,
        "PixelsAreNotAuthentic","`%s'",image->filename);
      return((Quantum *) NULL);
    }
  offset=(MagickOffsetType) y*cache_info->columns+x;
  if (offset < 0)
    return((Quantum *) NULL);
  number_pixels=(MagickSizeType) cache_info->columns*cache_info->rows;
  offset+=(MagickOffsetType) (rows-1)*cache_info->columns+columns-1;
  if ((MagickSizeType) offset >= number_pixels)
    return((Quantum *) NULL);
  /*
    Return pixel cache.
  */
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  pixels=SetPixelCacheNexusPixels(cache_info,WriteMode,&region,nexus_info,
    exception);
  return(pixels);
}