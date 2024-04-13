static inline void RelinquishCacheNexusPixels(NexusInfo *nexus_info)
{
  if (nexus_info->mapped == MagickFalse)
    (void) RelinquishAlignedMemory(nexus_info->cache);
  else
    (void) UnmapBlob(nexus_info->cache,(size_t) nexus_info->length);
  nexus_info->cache=(Quantum *) NULL;
  nexus_info->pixels=(Quantum *) NULL;
  nexus_info->metacontent=(void *) NULL;
  nexus_info->length=0;
  nexus_info->mapped=MagickFalse;
}