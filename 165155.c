static inline MagickBooleanType IsSameOpenCLDevice(MagickCLDevice a,
  MagickCLDevice b)
{
  if ((LocaleCompare(a->platform_name,b->platform_name) == 0) &&
      (LocaleCompare(a->vendor_name,b->vendor_name) == 0) &&
      (LocaleCompare(a->name,b->name) == 0) &&
      (LocaleCompare(a->version,b->version) == 0) &&
      (a->max_clock_frequency == b->max_clock_frequency) &&
      (a->max_compute_units == b->max_compute_units))
    return(MagickTrue);

  return(MagickFalse);
}