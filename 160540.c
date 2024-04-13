void LibRaw::setOlympusBodyFeatures(unsigned long long id)
{
  ilm.CamID = id;

  if ((id == 0x4434303430ULL) || // E-1
      (id == 0x4434303431ULL) || // E-300
      ((id & 0x00ffff0000ULL) == 0x0030300000ULL))
  {
    ilm.CameraFormat = LIBRAW_FORMAT_FT;

    if ((id == 0x4434303430ULL) || // E-1
        (id == 0x4434303431ULL) || // E-330
        ((id >= 0x5330303033ULL) && (id <= 0x5330303138ULL)) || // E-330 to E-520
        (id == 0x5330303233ULL) || // E-620
        (id == 0x5330303239ULL) || // E-450
        (id == 0x5330303330ULL) || // E-600
        (id == 0x5330303333ULL))   // E-5
    {
      ilm.CameraMount = LIBRAW_MOUNT_FT;
    }
    else
    {
      ilm.CameraMount = LIBRAW_MOUNT_mFT;
    }
  }
  else
  {
    ilm.LensMount = ilm.CameraMount = LIBRAW_MOUNT_FixedLens;
  }
  return;
}