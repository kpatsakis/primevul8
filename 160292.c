void LibRaw::parseOlympus_Equipment(unsigned tag, unsigned type, unsigned len,
                                    unsigned dng_writer)
{
  // uptag 2010

  int i;

  switch (tag)
  {
  case 0x0100:
    getOlympus_CameraType2();
    break;
  case 0x0101:
    if ((!imgdata.shootinginfo.BodySerial[0]) && (dng_writer == nonDNG))
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
    break;
  case 0x0102:
    stmread(imgdata.shootinginfo.InternalBodySerial, len, ifp);
    break;
  case 0x0201:
  {
    unsigned char bits[4];
    fread(bits, 1, 4, ifp);
    ilm.LensID = (unsigned long long)bits[0] << 16 |
                 (unsigned long long)bits[2] << 8 | (unsigned long long)bits[3];
    ilm.LensMount = LIBRAW_MOUNT_FT;
    ilm.LensFormat = LIBRAW_FORMAT_FT;
    if (((ilm.LensID < 0x20000) || (ilm.LensID > 0x4ffff)) &&
        (ilm.LensID & 0x10))
      ilm.LensMount = LIBRAW_MOUNT_mFT;
  }
  break;
  case 0x0202:
    if ((!imgdata.lens.LensSerial[0]))
      stmread(imgdata.lens.LensSerial, len, ifp);
    break;
  case 0x0203:
    stmread(ilm.Lens, len, ifp);
    break;
  case 0x0205:
    ilm.MaxAp4MinFocal = libraw_powf64l(sqrt(2.0f), get2() / 256.0f);
    break;
  case 0x0206:
    ilm.MaxAp4MaxFocal = libraw_powf64l(sqrt(2.0f), get2() / 256.0f);
    break;
  case 0x0207:
    ilm.MinFocal = (float)get2();
    break;
  case 0x0208:
    ilm.MaxFocal = (float)get2();
    if (ilm.MaxFocal > 1000.0f)
      ilm.MaxFocal = ilm.MinFocal;
    break;
  case 0x020a:
    ilm.MaxAp4CurFocal = libraw_powf64l(sqrt(2.0f), get2() / 256.0f);
    break;
  case 0x0301:
    ilm.TeleconverterID = fgetc(ifp) << 8;
    fgetc(ifp);
    ilm.TeleconverterID = ilm.TeleconverterID | fgetc(ifp);
    break;
  case 0x0303:
    stmread(ilm.Teleconverter, len, ifp);
    break;
  case 0x0403:
    stmread(ilm.Attachment, len, ifp);
    break;
  }

  return;
}