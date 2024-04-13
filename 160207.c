int LibRaw::set_rawspeed_camerafile(char *filename)
{
#ifdef USE_RAWSPEED
  try
  {
    CameraMetaDataLR *camerameta = new CameraMetaDataLR(filename);
    if (_rawspeed_camerameta)
    {
      CameraMetaDataLR *d =
          static_cast<CameraMetaDataLR *>(_rawspeed_camerameta);
      delete d;
    }
    _rawspeed_camerameta = static_cast<void *>(camerameta);
  }
  catch (...)
  {
    // just return error code
    return -1;
  }
#endif
  return 0;
}