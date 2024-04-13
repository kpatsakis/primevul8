static MagickBooleanType ReadPixelCachePixels(
  CacheInfo *magick_restrict cache_info,NexusInfo *magick_restrict nexus_info,
  ExceptionInfo *exception)
{
  MagickOffsetType
    count,
    offset;

  MagickSizeType
    extent,
    length;

  register Quantum
    *magick_restrict q;

  register ssize_t
    y;

  size_t
    number_channels,
    rows;

  if (nexus_info->authentic_pixel_cache != MagickFalse)
    return(MagickTrue);
  offset=(MagickOffsetType) nexus_info->region.y*cache_info->columns;
  if ((ssize_t) (offset/cache_info->columns) != nexus_info->region.y)
    return(MagickFalse);
  offset+=nexus_info->region.x;
  number_channels=cache_info->number_channels;
  length=(MagickSizeType) number_channels*nexus_info->region.width*
    sizeof(Quantum);
  if ((length/number_channels/sizeof(Quantum)) != nexus_info->region.width)
    return(MagickFalse);
  rows=nexus_info->region.height;
  extent=length*rows;
  if ((extent == 0) || ((extent/length) != rows))
    return(MagickFalse);
  y=0;
  q=nexus_info->pixels;
  switch (cache_info->type)
  {
    case MemoryCache:
    case MapCache:
    {
      register Quantum
        *magick_restrict p;

      /*
        Read pixels from memory.
      */
      if ((cache_info->columns == nexus_info->region.width) &&
          (extent == (MagickSizeType) ((size_t) extent)))
        {
          length=extent;
          rows=1UL;
        }
      p=cache_info->pixels+offset*cache_info->number_channels;
      for (y=0; y < (ssize_t) rows; y++)
      {
        (void) memcpy(q,p,(size_t) length);
        p+=cache_info->number_channels*cache_info->columns;
        q+=cache_info->number_channels*nexus_info->region.width;
      }
      break;
    }
    case DiskCache:
    {
      /*
        Read pixels from disk.
      */
      LockSemaphoreInfo(cache_info->file_semaphore);
      if (OpenPixelCacheOnDisk(cache_info,IOMode) == MagickFalse)
        {
          ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
            cache_info->cache_filename);
          UnlockSemaphoreInfo(cache_info->file_semaphore);
          return(MagickFalse);
        }
      if ((cache_info->columns == nexus_info->region.width) &&
          (extent <= MagickMaxBufferExtent))
        {
          length=extent;
          rows=1UL;
        }
      for (y=0; y < (ssize_t) rows; y++)
      {
        count=ReadPixelCacheRegion(cache_info,cache_info->offset+offset*
          cache_info->number_channels*sizeof(*q),length,(unsigned char *) q);
        if (count != (MagickOffsetType) length)
          break;
        offset+=cache_info->columns;
        q+=cache_info->number_channels*nexus_info->region.width;
      }
      if (IsFileDescriptorLimitExceeded() != MagickFalse)
        (void) ClosePixelCacheOnDisk(cache_info);
      UnlockSemaphoreInfo(cache_info->file_semaphore);
      break;
    }
    case DistributedCache:
    {
      RectangleInfo
        region;

      /*
        Read pixels from distributed cache.
      */
      LockSemaphoreInfo(cache_info->file_semaphore);
      region=nexus_info->region;
      if ((cache_info->columns != nexus_info->region.width) ||
          (extent > MagickMaxBufferExtent))
        region.height=1UL;
      else
        {
          length=extent;
          rows=1UL;
        }
      for (y=0; y < (ssize_t) rows; y++)
      {
        count=ReadDistributePixelCachePixels((DistributeCacheInfo *)
          cache_info->server_info,&region,length,(unsigned char *) q);
        if (count != (MagickOffsetType) length)
          break;
        q+=cache_info->number_channels*nexus_info->region.width;
        region.y++;
      }
      UnlockSemaphoreInfo(cache_info->file_semaphore);
      break;
    }
    default:
      break;
  }
  if (y < (ssize_t) rows)
    {
      ThrowFileException(exception,CacheError,"UnableToReadPixelCache",
        cache_info->cache_filename);
      return(MagickFalse);
    }
  if ((cache_info->debug != MagickFalse) &&
      (CacheTick(nexus_info->region.y,cache_info->rows) != MagickFalse))
    (void) LogMagickEvent(CacheEvent,GetMagickModule(),
      "%s[%.20gx%.20g%+.20g%+.20g]",cache_info->filename,(double)
      nexus_info->region.width,(double) nexus_info->region.height,(double)
      nexus_info->region.x,(double) nexus_info->region.y);
  return(MagickTrue);
}