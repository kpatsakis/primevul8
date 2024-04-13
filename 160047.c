int LibRaw::try_rawspeed()
{
#ifdef USE_RAWSPEED
  int ret = LIBRAW_SUCCESS;

  int rawspeed_ignore_errors = 0;
  if (imgdata.idata.dng_version && imgdata.idata.colors == 3 &&
      !strcasecmp(imgdata.idata.software,
                  "Adobe Photoshop Lightroom 6.1.1 (Windows)"))
    rawspeed_ignore_errors = 1;

  // RawSpeed Supported,
  INT64 spos = ID.input->tell();
  void *_rawspeed_buffer = 0;
  try
  {
    ID.input->seek(0, SEEK_SET);
    INT64 _rawspeed_buffer_sz = ID.input->size() + 32;
    _rawspeed_buffer = malloc(_rawspeed_buffer_sz);
    if (!_rawspeed_buffer)
      throw LIBRAW_EXCEPTION_ALLOC;
    ID.input->read(_rawspeed_buffer, _rawspeed_buffer_sz, 1);
    FileMap map((uchar8 *)_rawspeed_buffer, _rawspeed_buffer_sz);
    RawParser t(&map);
    RawDecoder *d = 0;
    CameraMetaDataLR *meta =
        static_cast<CameraMetaDataLR *>(_rawspeed_camerameta);
    d = t.getDecoder();
    if (!d)
      throw "Unable to find decoder";
    try
    {
      d->checkSupport(meta);
    }
    catch (const RawDecoderException &e)
    {
      imgdata.process_warnings |= LIBRAW_WARN_RAWSPEED_UNSUPPORTED;
      throw e;
    }
    d->interpolateBadPixels = FALSE;
    d->applyStage1DngOpcodes = FALSE;
    _rawspeed_decoder = static_cast<void *>(d);
    d->decodeRaw();
    d->decodeMetaData(meta);
    RawImage r = d->mRaw;
    if (r->errors.size() > 0 && !rawspeed_ignore_errors)
    {
      delete d;
      _rawspeed_decoder = 0;
      throw 1;
    }
    if (r->isCFA)
    {
      imgdata.rawdata.raw_image = (ushort *)r->getDataUncropped(0, 0);
    }
    else if (r->getCpp() == 4)
    {
      imgdata.rawdata.color4_image = (ushort(*)[4])r->getDataUncropped(0, 0);
      if (r->whitePoint > 0 && r->whitePoint < 65536)
        C.maximum = r->whitePoint;
    }
    else if (r->getCpp() == 3)
    {
      imgdata.rawdata.color3_image = (ushort(*)[3])r->getDataUncropped(0, 0);
      if (r->whitePoint > 0 && r->whitePoint < 65536)
        C.maximum = r->whitePoint;
    }
    else
    {
      delete d;
      _rawspeed_decoder = 0;
      ret = LIBRAW_UNSPECIFIED_ERROR;
    }
    if (_rawspeed_decoder)
    {
      // set sizes
      iPoint2D rsdim = r->getUncroppedDim();
      S.raw_pitch = r->pitch;
      S.raw_width = rsdim.x;
      S.raw_height = rsdim.y;
      // C.maximum = r->whitePoint;
      fix_after_rawspeed(r->blackLevel);
    }
    free(_rawspeed_buffer);
    _rawspeed_buffer = 0;
    imgdata.process_warnings |= LIBRAW_WARN_RAWSPEED_PROCESSED;
  }
  catch (const RawDecoderException &RDE)
  {
    imgdata.process_warnings |= LIBRAW_WARN_RAWSPEED_PROBLEM;
    if (_rawspeed_buffer)
    {
      free(_rawspeed_buffer);
      _rawspeed_buffer = 0;
    }
    const char *p = RDE.what();
    if (!strncmp(RDE.what(), "Decoder canceled", strlen("Decoder canceled")))
      throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
    ret = LIBRAW_UNSPECIFIED_ERROR;
  }
  catch (...)
  {
    // We may get here due to cancellation flag
    imgdata.process_warnings |= LIBRAW_WARN_RAWSPEED_PROBLEM;
    if (_rawspeed_buffer)
    {
      free(_rawspeed_buffer);
      _rawspeed_buffer = 0;
    }
    ret = LIBRAW_UNSPECIFIED_ERROR;
  }
  ID.input->seek(spos, SEEK_SET);

  return ret;
#else
  return LIBRAW_NOT_IMPLEMENTED;
#endif
}