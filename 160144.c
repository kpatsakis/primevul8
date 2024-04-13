void LibRaw::parseLeicaLensID()
{
  ilm.LensID = get4();
  if (ilm.LensID)
  {
    ilm.LensID = ((ilm.LensID >> 2) << 8) | (ilm.LensID & 0x3);
    if ((ilm.LensID > 0x00ff) && (ilm.LensID < 0x3b00))
    {
      ilm.LensMount = ilm.CameraMount;
      ilm.LensFormat = LIBRAW_FORMAT_FF;
    }
  }
}