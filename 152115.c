MagickPrivate NexusInfo **DestroyPixelCacheNexus(NexusInfo **nexus_info,
  const size_t number_threads)
{
  register ssize_t
    i;

  assert(nexus_info != (NexusInfo **) NULL);
  for (i=0; i < (ssize_t) number_threads; i++)
  {
    if (nexus_info[i]->cache != (Quantum *) NULL)
      RelinquishCacheNexusPixels(nexus_info[i]);
    nexus_info[i]->signature=(~MagickCoreSignature);
  }
  nexus_info[0]=(NexusInfo *) RelinquishMagickMemory(nexus_info[0]);
  nexus_info=(NexusInfo **) RelinquishAlignedMemory(nexus_info);
  return(nexus_info);
}