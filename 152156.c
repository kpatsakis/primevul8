MagickPrivate void ClonePixelCacheMethods(Cache clone,const Cache cache)
{
  CacheInfo
    *magick_restrict cache_info,
    *magick_restrict source_info;

  assert(clone != (Cache) NULL);
  source_info=(CacheInfo *) clone;
  assert(source_info->signature == MagickCoreSignature);
  if (source_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      source_info->filename);
  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickCoreSignature);
  source_info->methods=cache_info->methods;
}