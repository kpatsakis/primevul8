static inline MagickBooleanType IsPixelCacheAuthentic(
  const CacheInfo *magick_restrict cache_info,
  const NexusInfo *magick_restrict nexus_info)
{
  MagickBooleanType
    status;

  MagickOffsetType
    offset;

  /*
    Does nexus pixels point directly to in-core cache pixels or is it buffered?
  */
  if (cache_info->type == PingCache)
    return(MagickTrue);
  offset=(MagickOffsetType) nexus_info->region.y*cache_info->columns+
    nexus_info->region.x;
  status=nexus_info->pixels == (cache_info->pixels+offset*
    cache_info->number_channels) ? MagickTrue : MagickFalse;
  return(status);
}