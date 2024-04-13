void CLASS process_Sony_0x940c (uchar * buf)
{
  ushort lid2;
  if (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Canon_EF)
    {
      switch (SonySubstitution[buf[0x0008]]) {
      case 1:
      case 5:
        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Minolta_A;
        break;
      case 4:
        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E;
        break;
      }
    }
  lid2 = (((ushort)SonySubstitution[buf[0x000a]])<<8) |
    ((ushort)SonySubstitution[buf[0x0009]]);
  if ((lid2 > 0) && (lid2 < 32784))
    parseSonyLensType2 (SonySubstitution[buf[0x000a]],	// LensType2 - Sony lens ids
                        SonySubstitution[buf[0x0009]]);
  return;
}