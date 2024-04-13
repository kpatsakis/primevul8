void CLASS parseSonyLensType2 (uchar a, uchar b) {
  ushort lid2;
  lid2 = (((ushort)a)<<8) | ((ushort)b);
  if (!lid2) return;
  if (lid2 < 0x100)
    {
      imgdata.lens.makernotes.AdapterID = lid2;
      switch (lid2) {
      case 1:
      case 2:
      case 3:
      case 6:
        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Minolta_A;
        break;
      case 44:
      case 78:
      case 239:
        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
        break;
      }
    }
  else
    imgdata.lens.makernotes.LensID = lid2;
  return;
}