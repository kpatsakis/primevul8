void LibRaw::recycle()
{
  recycle_datastream();
#define FREE(a)                                                                \
  do                                                                           \
  {                                                                            \
    if (a)                                                                     \
    {                                                                          \
      free(a);                                                                 \
      a = NULL;                                                                \
    }                                                                          \
  } while (0)

  FREE(imgdata.image);

  FREE(imgdata.thumbnail.thumb);
  FREE(libraw_internal_data.internal_data.meta_data);
  FREE(libraw_internal_data.output_data.histogram);
  FREE(libraw_internal_data.output_data.oprof);
  FREE(imgdata.color.profile);
  FREE(imgdata.rawdata.ph1_cblack);
  FREE(imgdata.rawdata.ph1_rblack);
  FREE(imgdata.rawdata.raw_alloc);
  FREE(imgdata.idata.xmpdata);

#undef FREE

  ZERO(imgdata.sizes);
  imgdata.sizes.raw_inset_crop.cleft = 0xffff;
  imgdata.sizes.raw_inset_crop.ctop = 0xffff;

  ZERO(imgdata.idata);
  ZERO(imgdata.makernotes);
  ZERO(imgdata.color);
  ZERO(imgdata.other);
  ZERO(imgdata.thumbnail);
  ZERO(imgdata.rawdata);
  cleargps(&imgdata.other.parsed_gps);
  imgdata.color.dng_levels.baseline_exposure = -999.f;
  imgdata.color.dng_levels.LinearResponseLimit = 1.f;

  imgdata.makernotes.canon.SensorLeftBorder = -1;
  imgdata.makernotes.canon.SensorTopBorder = -1;

  imgdata.makernotes.nikon.SensorHighSpeedCrop.cleft = 0xffff;
  imgdata.makernotes.nikon.SensorHighSpeedCrop.ctop = 0xffff;

  imgdata.makernotes.fuji.ExpoMidPointShift = -999.f;
  imgdata.makernotes.fuji.DynamicRange = 0xffff;
  imgdata.makernotes.fuji.FilmMode = 0xffff;
  imgdata.makernotes.fuji.DynamicRangeSetting = 0xffff;
  imgdata.makernotes.fuji.DevelopmentDynamicRange = 0xffff;
  imgdata.makernotes.fuji.AutoDynamicRange = 0xffff;
  imgdata.makernotes.fuji.FocusMode = 0xffff;
  imgdata.makernotes.fuji.DriveMode = -1;
  imgdata.makernotes.fuji.AFMode = 0xffff;
  imgdata.makernotes.fuji.FocusPixel[0] =
      imgdata.makernotes.fuji.FocusPixel[1] = 0xffff;
  imgdata.makernotes.fuji.ImageStabilization[0] =
      imgdata.makernotes.fuji.ImageStabilization[1] =
          imgdata.makernotes.fuji.ImageStabilization[2] = 0xffff;

  imgdata.makernotes.samsung.ColorSpace[0] =
      imgdata.makernotes.samsung.ColorSpace[1] = -1;

  imgdata.makernotes.sony.CameraType = 0xffff;
  imgdata.makernotes.sony.real_iso_offset = 0xffff;
  imgdata.makernotes.sony.ImageCount3_offset = 0xffff;
  imgdata.makernotes.sony.ElectronicFrontCurtainShutter = 0xffff;
  imgdata.makernotes.sony.MinoltaCamID = -1;
  imgdata.makernotes.sony.RAWFileType = 0xffff;
  imgdata.makernotes.sony.AFAreaModeSetting = 0xff;
  imgdata.makernotes.sony.FlexibleSpotPosition[0] =
      imgdata.makernotes.sony.FlexibleSpotPosition[1] = 0xffff;
  imgdata.makernotes.sony.AFPointSelected = 0xff;
  imgdata.makernotes.sony.LongExposureNoiseReduction = 0xffffffff;
  imgdata.makernotes.sony.Quality = 0xffffffff;
  imgdata.makernotes.sony.HighISONoiseReduction = 0xffff;
  imgdata.makernotes.sony.SonyRawFileType = 0xffff;

  imgdata.makernotes.kodak.BlackLevelTop = 0xffff;
  imgdata.makernotes.kodak.BlackLevelBottom = 0xffff;
  imgdata.makernotes.kodak.ISOCalibrationGain = 1.0f;

  imgdata.makernotes.hasselblad.nIFD_CM[0] =
    imgdata.makernotes.hasselblad.nIFD_CM[1] = -1;

  imgdata.color.dng_color[0].illuminant =
      imgdata.color.dng_color[1].illuminant = LIBRAW_WBI_None;

  for (int i = 0; i < 4; i++)
    imgdata.color.dng_levels.analogbalance[i] = 1.0f;

  ZERO(libraw_internal_data);
  ZERO(imgdata.lens);
  imgdata.lens.makernotes.FocalUnits = 1;
  imgdata.lens.makernotes.LensID = 0xffffffffffffffffULL;
  ZERO(imgdata.shootinginfo);
  imgdata.shootinginfo.DriveMode = -1;
  imgdata.shootinginfo.FocusMode = -1;
  imgdata.shootinginfo.MeteringMode = -1;
  imgdata.shootinginfo.AFPoint = -1;
  imgdata.shootinginfo.ExposureMode = -1;
  imgdata.shootinginfo.ExposureProgram = -1;
  imgdata.shootinginfo.ImageStabilization = -1;

  _exitflag = 0;
#ifdef USE_RAWSPEED
  if (_rawspeed_decoder)
  {
    RawSpeed::RawDecoder *d =
        static_cast<RawSpeed::RawDecoder *>(_rawspeed_decoder);
    delete d;
  }
  _rawspeed_decoder = 0;
#endif
#ifdef USE_DNGSDK
  if (dngnegative)
  {
    dng_negative *ng = (dng_negative *)dngnegative;
    delete ng;
    dngnegative = 0;
  }
  if(dngimage)
  {
      dng_image *dimage = (dng_image*)dngimage;
      delete dimage;
      dngimage = 0;
  }
#endif
  if (_x3f_data)
  {
    x3f_clear(_x3f_data);
    _x3f_data = 0;
  }

  memmgr.cleanup();

  imgdata.thumbnail.tformat = LIBRAW_THUMBNAIL_UNKNOWN;
  imgdata.progress_flags = 0;

  load_raw = thumb_load_raw = 0;

  tls->init();
}