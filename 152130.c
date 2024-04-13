static inline MagickBooleanType ValidatePixelCacheMorphology(
  const Image *magick_restrict image)
{
  const CacheInfo
    *magick_restrict cache_info;

  const PixelChannelMap
    *magick_restrict p,
    *magick_restrict q;

  /*
    Does the image match the pixel cache morphology?
  */
  cache_info=(CacheInfo *) image->cache;
  p=image->channel_map;
  q=cache_info->channel_map;
  if ((image->storage_class != cache_info->storage_class) ||
      (image->colorspace != cache_info->colorspace) ||
      (image->alpha_trait != cache_info->alpha_trait) ||
      (image->read_mask != cache_info->read_mask) ||
      (image->write_mask != cache_info->write_mask) ||
      (image->columns != cache_info->columns) ||
      (image->rows != cache_info->rows) ||
      (image->number_channels != cache_info->number_channels) ||
      (memcmp(p,q,image->number_channels*sizeof(*p)) != 0) ||
      (image->metacontent_extent != cache_info->metacontent_extent) ||
      (cache_info->nexus_info == (NexusInfo **) NULL))
    return(MagickFalse);
  return(MagickTrue);
}