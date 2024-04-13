static MagickBooleanType ReadPixelCacheMetacontent(
  CacheInfo *magick_restrict cache_info,NexusInfo *magick_restrict nexus_info,
  ExceptionInfo *exception)
{
  MagickOffsetType
    count,
    offset;

  MagickSizeType
    extent,
    length;

  register ssize_t
    y;

  register unsigned char
    *magick_restrict q;

  size_t
    rows;

  if (cache_info->metacontent_extent == 0)
    return(MagickFalse);
  if (nexus_info->authentic_pixel_cache != MagickFalse)
    return(MagickTrue);
  offset=(MagickOffsetType) nexus_info->region.y*cache_info->columns+
    nexus_info->region.x;
  length=(MagickSizeType) nexus_info->region.width*
    cache_info->metacontent_extent;
  extent=length*nexus_info->region.height;
  rows=nexus_info->region.height;
  y=0;
  q=(unsigned char *) nexus_info->metacontent;
  switch (cache_info->type)
  {
    case MemoryCache:
    case MapCache:
    {
      register unsigned char
        *magick_restrict p;

      /*
        Read meta-content from memory.
      */
      if ((cache_info->columns == nexus_info->region.width) &&
          (extent == (MagickSizeType) ((size_t) extent)))
        {
          length=extent;
          rows=1UL;
        }
      p=(unsigned char *) cache_info->metacontent+offset*
        cache_info->metacontent_extent;
      for (y=0; y < (ssize_t) rows; y++)
      {
        (void) memcpy(q,p,(size_t) length);
        p+=cache_info->metacontent_extent*cache_info->columns;
        q+=cache_info->metacontent_extent*nexus_info->region.width;
      }
      break;
    }
    case DiskCache:
    {
      /*
        Read meta content from disk.
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
      extent=(MagickSizeType) cache_info->columns*cache_info->rows;
      for (y=0; y < (ssize_t) rows; y++)
      {
        count=ReadPixelCacheRegion(cache_info,cache_info->offset+extent*
          cache_info->number_channels*sizeof(Quantum)+offset*
          cache_info->metacontent_extent,length,(unsigned char *) q);
        if (count != (MagickOffsetType) length)
          break;
        offset+=cache_info->columns;
        q+=cache_info->metacontent_extent*nexus_info->region.width;
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
        Read metacontent from distributed cache.
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
        count=ReadDistributePixelCacheMetacontent((DistributeCacheInfo *)
          cache_info->server_info,&region,length,(unsigned char *) q);
        if (count != (MagickOffsetType) length)
          break;
        q+=cache_info->metacontent_extent*nexus_info->region.width;
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