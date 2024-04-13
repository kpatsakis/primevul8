void LibRaw::process_Sony_0x940e(uchar *buf, ushort len, unsigned long long id)
{
  if (((imSony.CameraType != LIBRAW_SONY_SLT) &&
       (imSony.CameraType != LIBRAW_SONY_ILCA)) ||
      (id == 0x118ULL) || // SLT-A33
      (id == 0x119ULL) || // SLT-A55V
      (id == 0x11dULL) || // SLT-A35
      (len < 3))
    return;

  imSony.AFType = SonySubstitution[buf[0x02]];

  if (imSony.CameraType == LIBRAW_SONY_ILCA)
  {
    if (len >= 0x06)
    {
      imgdata.shootinginfo.FocusMode = SonySubstitution[buf[0x05]];
    }
    if (len >= 0x0051)
    {
      imSony.AFMicroAdjValue = SonySubstitution[buf[0x0050]];
    }
  }
  else
  {
    if (len >= 0x0c)
    {
      imgdata.shootinginfo.FocusMode = SonySubstitution[buf[0x0b]];
    }
    if (len >= 0x017e)
    {
      imSony.AFMicroAdjValue = SonySubstitution[buf[0x017d]];
    }
  }

  if (imSony.AFMicroAdjValue != 0)
    imSony.AFMicroAdjOn = 1;
}