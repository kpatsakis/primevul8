MagickExport const char *GetOpenCLDeviceVendorName(const MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return((const char *) NULL);
  return(device->vendor_name);
}