static void CL_API_CALL DestroyMagickCLCacheInfoAndPixels(
  cl_event magick_unused(event),
  cl_int magick_unused(event_command_exec_status),void *user_data)
{
  MagickCLCacheInfo
    info;

  Quantum
    *pixels;

  magick_unreferenced(event);
  magick_unreferenced(event_command_exec_status);
  info=(MagickCLCacheInfo) user_data;
  pixels=info->pixels;
  RelinquishMagickResource(MemoryResource,info->length);
  DestroyMagickCLCacheInfo(info);
  (void) RelinquishAlignedMemory(pixels);
}