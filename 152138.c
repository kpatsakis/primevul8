static MagickBooleanType ClonePixelCacheOnDisk(
  CacheInfo *magick_restrict cache_info,CacheInfo *magick_restrict clone_info)
{
  MagickSizeType
    extent;

  size_t
    quantum;

  ssize_t
    count;

  struct stat
    file_stats;

  unsigned char
    *buffer;

  /*
    Clone pixel cache on disk with identical morphology.
  */
  if ((OpenPixelCacheOnDisk(cache_info,ReadMode) == MagickFalse) ||
      (OpenPixelCacheOnDisk(clone_info,IOMode) == MagickFalse))
    return(MagickFalse);
  quantum=(size_t) MagickMaxBufferExtent;
  if ((fstat(cache_info->file,&file_stats) == 0) && (file_stats.st_size > 0))
    quantum=(size_t) MagickMin(file_stats.st_size,MagickMaxBufferExtent);
  buffer=(unsigned char *) AcquireQuantumMemory(quantum,sizeof(*buffer));
  if (buffer == (unsigned char *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  extent=0;
  while ((count=read(cache_info->file,buffer,quantum)) > 0)
  {
    ssize_t
      number_bytes;

    number_bytes=write(clone_info->file,buffer,(size_t) count);
    if (number_bytes != count)
      break;
    extent+=number_bytes;
  }
  buffer=(unsigned char *) RelinquishMagickMemory(buffer);
  if (extent != cache_info->length)
    return(MagickFalse);
  return(MagickTrue);
}