static MagickBooleanType OpenPixelCacheOnDisk(CacheInfo *cache_info,
  const MapMode mode)
{
  int
    file;

  /*
    Open pixel cache on disk.
  */
  if ((cache_info->file != -1) && (cache_info->mode == mode))
    return(MagickTrue);  /* cache already open and in the proper mode */
  if (*cache_info->cache_filename == '\0')
    file=AcquireUniqueFileResource(cache_info->cache_filename);
  else
    switch (mode)
    {
      case ReadMode:
      {
        file=open_utf8(cache_info->cache_filename,O_RDONLY | O_BINARY,0);
        break;
      }
      case WriteMode:
      {
        file=open_utf8(cache_info->cache_filename,O_WRONLY | O_CREAT |
          O_BINARY | O_EXCL,S_MODE);
        if (file == -1)
          file=open_utf8(cache_info->cache_filename,O_WRONLY | O_BINARY,S_MODE);
        break;
      }
      case IOMode:
      default:
      {
        file=open_utf8(cache_info->cache_filename,O_RDWR | O_CREAT | O_BINARY |
          O_EXCL,S_MODE);
        if (file == -1)
          file=open_utf8(cache_info->cache_filename,O_RDWR | O_BINARY,S_MODE);
        break;
      }
    }
  if (file == -1)
    return(MagickFalse);
  (void) AcquireMagickResource(FileResource,1);
  if (cache_info->file != -1)
    (void) ClosePixelCacheOnDisk(cache_info);
  cache_info->file=file;
  return(MagickTrue);
}