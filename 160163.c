LibRaw::LibRaw(unsigned int flags) : memmgr(1024)
{
  double aber[4] = {1, 1, 1, 1};
  double gamm[6] = {0.45, 4.5, 0, 0, 0, 0};
  unsigned greybox[4] = {0, 0, UINT_MAX, UINT_MAX};
  unsigned cropbox[4] = {0, 0, UINT_MAX, UINT_MAX};
  ZERO(imgdata);

  cleargps(&imgdata.other.parsed_gps);
  ZERO(libraw_internal_data);
  ZERO(callbacks);

  _rawspeed_camerameta = _rawspeed_decoder = NULL;
  dnghost = NULL;
  dngnegative = NULL;
  dngimage = NULL;
  _x3f_data = NULL;

#ifdef USE_RAWSPEED
  CameraMetaDataLR *camerameta =
      make_camera_metadata(); // May be NULL in case of exception in
                              // make_camera_metadata()
  _rawspeed_camerameta = static_cast<void *>(camerameta);
#endif
  callbacks.mem_cb = (flags & LIBRAW_OPIONS_NO_MEMERR_CALLBACK)
                         ? NULL
                         : &default_memory_callback;
  callbacks.data_cb = (flags & LIBRAW_OPIONS_NO_DATAERR_CALLBACK)
                          ? NULL
                          : &default_data_callback;
  callbacks.exif_cb = NULL; // no default callback
  callbacks.pre_identify_cb = NULL;
  callbacks.post_identify_cb = NULL;
  callbacks.pre_subtractblack_cb = callbacks.pre_scalecolors_cb =
      callbacks.pre_preinterpolate_cb = callbacks.pre_interpolate_cb =
          callbacks.interpolate_bayer_cb = callbacks.interpolate_xtrans_cb =
              callbacks.post_interpolate_cb = callbacks.pre_converttorgb_cb =
                  callbacks.post_converttorgb_cb = NULL;

  memmove(&imgdata.params.aber, &aber, sizeof(aber));
  memmove(&imgdata.params.gamm, &gamm, sizeof(gamm));
  memmove(&imgdata.params.greybox, &greybox, sizeof(greybox));
  memmove(&imgdata.params.cropbox, &cropbox, sizeof(cropbox));

  imgdata.params.bright = 1;
  imgdata.params.use_camera_matrix = 1;
  imgdata.params.user_flip = -1;
  imgdata.params.user_black = -1;
  imgdata.params.user_cblack[0] = imgdata.params.user_cblack[1] =
      imgdata.params.user_cblack[2] = imgdata.params.user_cblack[3] = -1000001;
  imgdata.params.user_sat = -1;
  imgdata.params.user_qual = -1;
  imgdata.params.output_color = 1;
  imgdata.params.output_bps = 8;
  imgdata.params.use_fuji_rotate = 1;
  imgdata.params.exp_shift = 1.0;
  imgdata.params.auto_bright_thr = LIBRAW_DEFAULT_AUTO_BRIGHTNESS_THRESHOLD;
  imgdata.params.adjust_maximum_thr = LIBRAW_DEFAULT_ADJUST_MAXIMUM_THRESHOLD;
  imgdata.params.use_rawspeed = 1;
  imgdata.params.use_dngsdk = LIBRAW_DNG_DEFAULT;
  imgdata.params.no_auto_scale = 0;
  imgdata.params.no_interpolation = 0;
  imgdata.params.raw_processing_options = LIBRAW_PROCESSING_DP2Q_INTERPOLATERG |
                                          LIBRAW_PROCESSING_DP2Q_INTERPOLATEAF |
                                          LIBRAW_PROCESSING_CONVERTFLOAT_TO_INT;
  imgdata.params.sony_arw2_posterization_thr = 0;
  imgdata.params.max_raw_memory_mb = LIBRAW_MAX_ALLOC_MB_DEFAULT;
  imgdata.params.green_matching = 0;
  imgdata.params.custom_camera_strings = 0;
  imgdata.params.coolscan_nef_gamma = 1.0f;
  imgdata.parent_class = this;
  imgdata.progress_flags = 0;
  imgdata.color.dng_levels.baseline_exposure = -999.f;
  imgdata.color.dng_levels.LinearResponseLimit = 1.0f;
  imgdata.makernotes.hasselblad.nIFD_CM[0] =
    imgdata.makernotes.hasselblad.nIFD_CM[1] = -1;
  imgdata.makernotes.kodak.ISOCalibrationGain = 1.0f;
  _exitflag = 0;
  tls = new LibRaw_TLS;
  tls->init();
}