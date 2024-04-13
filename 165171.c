MagickPrivate void DumpOpenCLProfileData()
{
#define OpenCLLog(message) \
   fwrite(message,sizeof(char),strlen(message),log); \
   fwrite("\n",sizeof(char),1,log);

  char
    buf[4096],
    filename[MagickPathExtent],
    indent[160];

  FILE
    *log;

  MagickCLEnv
    clEnv;

  size_t
    i,
    j;

  clEnv=GetCurrentOpenCLEnv();
  if (clEnv == (MagickCLEnv) NULL)
    return;

  for (i = 0; i < clEnv->number_devices; i++)
    if (clEnv->devices[i]->profile_kernels != MagickFalse)
      break;
  if (i == clEnv->number_devices)
    return;

  (void) FormatLocaleString(filename,MagickPathExtent,"%s%s%s",
    GetOpenCLCacheDirectory(),DirectorySeparator,"ImageMagickOpenCL.log");

  log=fopen_utf8(filename,"wb");

  for (i = 0; i < clEnv->number_devices; i++)
  {
    MagickCLDevice
      device;

    device=clEnv->devices[i];
    if ((device->profile_kernels == MagickFalse) ||
        (device->profile_records == (KernelProfileRecord *) NULL))
      continue;

    OpenCLLog("====================================================");
    fprintf(log,"Device:  %s\n",device->name);
    fprintf(log,"Version: %s\n",device->version);
    OpenCLLog("====================================================");
    OpenCLLog("                     average   calls     min     max");
    OpenCLLog("                     -------   -----     ---     ---");
    j=0;
    while (device->profile_records[j] != (KernelProfileRecord) NULL)
    {
      KernelProfileRecord
        profile;

      profile=device->profile_records[j];
      strcpy(indent,"                    ");
      strncpy(indent,profile->kernel_name,MagickMin(strlen(
        profile->kernel_name),strlen(indent)-1));
      sprintf(buf,"%s %7d %7d %7d %7d",indent,(int) (profile->total/
        profile->count),(int) profile->count,(int) profile->min,
        (int) profile->max);
      OpenCLLog(buf);
      j++;
    }
    OpenCLLog("====================================================");
    fwrite("\n\n",sizeof(char),2,log);
  }
  fclose(log);
}