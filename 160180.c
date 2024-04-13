CameraMetaDataLR *make_camera_metadata()
{
  int len = 0, i;
  for (i = 0; i < RAWSPEED_DATA_COUNT; i++)
    if (_rawspeed_data_xml[i])
    {
      len += strlen(_rawspeed_data_xml[i]);
    }
  char *rawspeed_xml =
      (char *)calloc(len + 1, sizeof(_rawspeed_data_xml[0][0]));
  if (!rawspeed_xml)
    return NULL;
  int offt = 0;
  for (i = 0; i < RAWSPEED_DATA_COUNT; i++)
    if (_rawspeed_data_xml[i])
    {
      int ll = strlen(_rawspeed_data_xml[i]);
      if (offt + ll > len)
        break;
      memmove(rawspeed_xml + offt, _rawspeed_data_xml[i], ll);
      offt += ll;
    }
  rawspeed_xml[offt] = 0;
  CameraMetaDataLR *ret = NULL;
  try
  {
    ret = new CameraMetaDataLR(rawspeed_xml, offt);
  }
  catch (...)
  {
    // Mask all exceptions
  }
  free(rawspeed_xml);
  return ret;
}