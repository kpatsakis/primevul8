MagickPrivate NexusInfo **AcquirePixelCacheNexus(const size_t number_threads)
{
  NexusInfo
    **magick_restrict nexus_info;

  register ssize_t
    i;

  nexus_info=(NexusInfo **) MagickAssumeAligned(AcquireAlignedMemory(
    number_threads,sizeof(*nexus_info)));
  if (nexus_info == (NexusInfo **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  nexus_info[0]=(NexusInfo *) AcquireQuantumMemory(number_threads,
    sizeof(**nexus_info));
  if (nexus_info[0] == (NexusInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(nexus_info[0],0,number_threads*sizeof(**nexus_info));
  for (i=0; i < (ssize_t) number_threads; i++)
  {
    nexus_info[i]=(&nexus_info[0][i]);
    nexus_info[i]->signature=MagickCoreSignature;
  }
  return(nexus_info);
}