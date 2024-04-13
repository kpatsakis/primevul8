unsigned LibRaw::capabilities()
{
  unsigned ret = 0;
#ifdef USE_RAWSPEED
  ret |= LIBRAW_CAPS_RAWSPEED;
#endif
#ifdef USE_DNGSDK
  ret |= LIBRAW_CAPS_DNGSDK;
#ifdef USE_GPRSDK
  ret |= LIBRAW_CAPS_GPRSDK;
#endif
#ifdef LIBRAW_WIN32_UNICODEPATHS
  ret |= LIBRAW_CAPS_UNICODEPATHS;
#endif
#endif
  return ret;
}