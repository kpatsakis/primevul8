static MagickBooleanType ClosePixelCacheOnDisk(CacheInfo *cache_info)
{
  int
    status;

  status=(-1);
  if (cache_info->file != -1)
    {
      status=close(cache_info->file);
      cache_info->file=(-1);
      RelinquishMagickResource(FileResource,1);
    }
  return(status == -1 ? MagickFalse : MagickTrue);
}