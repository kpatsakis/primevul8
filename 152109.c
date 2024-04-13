static Quantum *SetPixelCacheNexusPixels(const CacheInfo *cache_info,
  const MapMode mode,const RectangleInfo *region,NexusInfo *nexus_info,
  ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  MagickSizeType
    length,
    number_pixels;

  assert(cache_info != (const CacheInfo *) NULL);
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->type == UndefinedCache)
    return((Quantum *) NULL);
  if ((region->width == 0) || (region->height == 0))
    return((Quantum *) NULL);
  nexus_info->region=(*region);
  number_pixels=(MagickSizeType) nexus_info->region.width*
    nexus_info->region.height;
  if (number_pixels == 0)
    return((Quantum *) NULL);
  if ((cache_info->type == MemoryCache) || (cache_info->type == MapCache))
    {
      ssize_t
        x,
        y;

      x=nexus_info->region.x+(ssize_t) nexus_info->region.width-1;
      y=nexus_info->region.y+(ssize_t) nexus_info->region.height-1;
      if (((nexus_info->region.x >= 0) && (x < (ssize_t) cache_info->columns) &&
           (nexus_info->region.y >= 0) && (y < (ssize_t) cache_info->rows)) &&
          ((nexus_info->region.height == 1UL) || ((nexus_info->region.x == 0) &&
           ((nexus_info->region.width == cache_info->columns) ||
            ((nexus_info->region.width % cache_info->columns) == 0)))))
        {
          MagickOffsetType
            offset;

          /*
            Pixels are accessed directly from memory.
          */
          offset=(MagickOffsetType) nexus_info->region.y*cache_info->columns+
            nexus_info->region.x;
          nexus_info->pixels=cache_info->pixels+cache_info->number_channels*
            offset;
          nexus_info->metacontent=(void *) NULL;
          if (cache_info->metacontent_extent != 0)
            nexus_info->metacontent=(unsigned char *) cache_info->metacontent+
              offset*cache_info->metacontent_extent;
          PrefetchPixelCacheNexusPixels(nexus_info,mode);
          nexus_info->authentic_pixel_cache=IsPixelCacheAuthentic(cache_info,
            nexus_info);
          return(nexus_info->pixels);
        }
    }
  /*
    Pixels are stored in a staging region until they are synced to the cache.
  */
  length=number_pixels*cache_info->number_channels*sizeof(Quantum);
  if (cache_info->metacontent_extent != 0)
    length+=number_pixels*cache_info->metacontent_extent;
  if (nexus_info->cache == (Quantum *) NULL)
    {
      nexus_info->length=length;
      status=AcquireCacheNexusPixels(cache_info,nexus_info,exception);
      if (status == MagickFalse)
        {
          nexus_info->length=0;
          return((Quantum *) NULL);
        }
    }
  else
    if (nexus_info->length < length)
      {
        RelinquishCacheNexusPixels(nexus_info);
        nexus_info->length=length;
        status=AcquireCacheNexusPixels(cache_info,nexus_info,exception);
        if (status == MagickFalse)
          {
            nexus_info->length=0;
            return((Quantum *) NULL);
          }
      }
  nexus_info->pixels=nexus_info->cache;
  nexus_info->metacontent=(void *) NULL;
  if (cache_info->metacontent_extent != 0)
    nexus_info->metacontent=(void *) (nexus_info->pixels+number_pixels*
      cache_info->number_channels);
  PrefetchPixelCacheNexusPixels(nexus_info,mode);
  nexus_info->authentic_pixel_cache=IsPixelCacheAuthentic(cache_info,
    nexus_info);
  return(nexus_info->pixels);
}