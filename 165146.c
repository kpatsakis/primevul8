static void LoadOpenCLDeviceBenchmark(MagickCLEnv clEnv,const char *xml)
{
  char
    keyword[MagickPathExtent],
    *token;

  const char
    *q;

  MagickCLDeviceBenchmark
    *device_benchmark;

  size_t
    i,
    extent;

  if (xml == (char *) NULL)
    return;
  device_benchmark=(MagickCLDeviceBenchmark *) NULL;
  token=AcquireString(xml);
  extent=strlen(token)+MagickPathExtent;
  for (q=(char *) xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetNextToken(q,&q,extent,token);
    if (*token == '\0')
      break;
    (void) CopyMagickString(keyword,token,MagickPathExtent);
    if (LocaleNCompare(keyword,"<!DOCTYPE",9) == 0)
      {
        /*
          Doctype element.
        */
        while ((LocaleNCompare(q,"]>",2) != 0) && (*q != '\0'))
          GetNextToken(q,&q,extent,token);
        continue;
      }
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetNextToken(q,&q,extent,token);
        continue;
      }
    if (LocaleCompare(keyword,"<device") == 0)
      {
        /*
          Device element.
        */
        device_benchmark=(MagickCLDeviceBenchmark *) AcquireMagickMemory(
          sizeof(*device_benchmark));
        if (device_benchmark == (MagickCLDeviceBenchmark *) NULL)
          break;
        (void) ResetMagickMemory(device_benchmark,0,sizeof(*device_benchmark));
        device_benchmark->score=MAGICKCORE_OPENCL_UNDEFINED_SCORE;
        continue;
      }
    if (device_benchmark == (MagickCLDeviceBenchmark *) NULL)
      continue;
    if (LocaleCompare(keyword,"/>") == 0)
      {
        if (device_benchmark->score != MAGICKCORE_OPENCL_UNDEFINED_SCORE)
          {
            if (LocaleCompare(device_benchmark->name, "CPU") == 0)
              clEnv->cpu_score=device_benchmark->score;
            else
              {
                MagickCLDevice
                  device;

                /*
                  Set the score for all devices that match this device.
                */
                for (i = 0; i < clEnv->number_devices; i++)
                {
                  device=clEnv->devices[i];
                  if (IsBenchmarkedOpenCLDevice(device,device_benchmark))
                    device->score=device_benchmark->score;
                }
              }
          }

        device_benchmark->platform_name=RelinquishMagickMemory(
          device_benchmark->platform_name);
        device_benchmark->vendor_name=RelinquishMagickMemory(
          device_benchmark->vendor_name);
        device_benchmark->name=RelinquishMagickMemory(device_benchmark->name);
        device_benchmark->version=RelinquishMagickMemory(
          device_benchmark->version);
        device_benchmark=(MagickCLDeviceBenchmark *) RelinquishMagickMemory(
          device_benchmark);
        continue;
      }
    GetNextToken(q,(const char **) NULL,extent,token);
    if (*token != '=')
      continue;
    GetNextToken(q,&q,extent,token);
    GetNextToken(q,&q,extent,token);
    switch (*keyword)
    {
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"maxClockFrequency") == 0)
          {
            device_benchmark->max_clock_frequency=StringToInteger(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"maxComputeUnits") == 0)
          {
            device_benchmark->max_compute_units=StringToInteger(token);
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          device_benchmark->name=ConstantString(token);
        break;
      }
      case 'P':
      case 'p':
      {
        if (LocaleCompare((char *) keyword,"platform") == 0)
          device_benchmark->platform_name=ConstantString(token);
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"score") == 0)
          device_benchmark->score=StringToDouble(token,(char **) NULL);
        break;
      }
      case 'V':
      case 'v':
      {
        if (LocaleCompare((char *) keyword,"vendor") == 0)
          device_benchmark->vendor_name=ConstantString(token);
        if (LocaleCompare((char *) keyword,"version") == 0)
          device_benchmark->version=ConstantString(token);
        break;
      }
      default:
        break;
    }
  }
  token=(char *) RelinquishMagickMemory(token);
  device_benchmark=(MagickCLDeviceBenchmark *) RelinquishMagickMemory(
    device_benchmark);
}