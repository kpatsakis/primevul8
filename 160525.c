void LibRaw::process_Sony_0x940c(uchar *buf, ushort len)
{
  if ((imSony.CameraType != LIBRAW_SONY_ILCE) &&
      (imSony.CameraType != LIBRAW_SONY_NEX))
    return;
  if (len <= 0x000a)
    return;

  ushort lid2;
  if ((ilm.LensMount != LIBRAW_MOUNT_Canon_EF) &&
      (ilm.LensMount != LIBRAW_MOUNT_Sigma_X3F))
  {
    switch (SonySubstitution[buf[0x0008]])
    {
    case 1:
    case 5:
      ilm.LensMount = LIBRAW_MOUNT_Minolta_A;
      break;
    case 4:
      ilm.LensMount = LIBRAW_MOUNT_Sony_E;
      break;
    }
  }
  lid2 = (((ushort)SonySubstitution[buf[0x000a]]) << 8) |
         ((ushort)SonySubstitution[buf[0x0009]]);
  if ((lid2 > 0) &&
      ((lid2 < 32784) || (ilm.LensID == 0x1999) || (ilm.LensID == 0xffff)))
    parseSonyLensType2(
        SonySubstitution[buf[0x000a]], // LensType2 - Sony lens ids
        SonySubstitution[buf[0x0009]]);
  if ((lid2 == 44) || (lid2 == 78) || (lid2 == 184) || (lid2 == 234) ||
      (lid2 == 239))
    ilm.AdapterID = lid2;
  return;
}