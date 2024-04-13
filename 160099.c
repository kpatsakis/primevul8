LibRaw::~LibRaw()
{
  recycle();
  delete tls;
#ifdef USE_RAWSPEED
  if (_rawspeed_camerameta)
  {
    CameraMetaDataLR *cmeta =
        static_cast<CameraMetaDataLR *>(_rawspeed_camerameta);
    delete cmeta;
    _rawspeed_camerameta = NULL;
  }
#endif
}