int LibRaw::valid_for_dngsdk()
{
#ifndef USE_DNGSDK
  return 0;
#else
  if (!imgdata.idata.dng_version)
    return 0;
#ifdef USE_GPRSDK
  if (load_raw == &LibRaw::vc5_dng_load_raw_placeholder) // regardless of flags or use_dngsdk value!
      return 1;
#endif
  if (!imgdata.params.use_dngsdk)
    return 0;
  if (load_raw == &LibRaw::lossy_dng_load_raw) // WHY??
    return 0;
  if (load_raw ==
      &LibRaw::float_dng_load_raw_placeholder) // regardless of flags!
    return 1;
  if (is_floating_point() && (imgdata.params.use_dngsdk & LIBRAW_DNG_FLOAT))
    return 1;
  if (!imgdata.idata.filters && (imgdata.params.use_dngsdk & LIBRAW_DNG_LINEAR))
    return 1;
  if (libraw_internal_data.unpacker_data.tiff_bps == 8 &&
      (imgdata.params.use_dngsdk & LIBRAW_DNG_8BIT))
    return 1;
  if (libraw_internal_data.unpacker_data.tiff_compress == 8 &&
      (imgdata.params.use_dngsdk & LIBRAW_DNG_DEFLATE))
    return 1;
  if (libraw_internal_data.unpacker_data.tiff_samples == 2)
    return 0; // Always deny 2-samples (old fuji superccd)
  if (imgdata.idata.filters == 9 &&
      (imgdata.params.use_dngsdk & LIBRAW_DNG_XTRANS))
    return 1;
  if (is_fuji_rotated())
    return 0; // refuse
  if (imgdata.params.use_dngsdk & LIBRAW_DNG_OTHER)
    return 1;
  return 0;
#endif
}