void CLASS parseOlympus_Equipment (unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
// uptag 2010

  int i;

  switch (tag) {
  case 0x0100:
    getOlympus_CameraType2();
    break;
  case 0x0101:
    if ((!imgdata.shootinginfo.BodySerial[0]) &&
        (dng_writer == nonDNG))
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
    break;
  case 0x0102:
    stmread(imgdata.shootinginfo.InternalBodySerial, len, ifp);
    break;
  case 0x0201:
    imgdata.lens.makernotes.LensID =
      (unsigned long long)fgetc(ifp) << 16 |
      (unsigned long long)(fgetc(ifp), fgetc(ifp)) << 8 |
      (unsigned long long)fgetc(ifp);
    imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FT;
    imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_FT;
    if (((imgdata.lens.makernotes.LensID < 0x20000) ||
         (imgdata.lens.makernotes.LensID > 0x4ffff)) &&
        (imgdata.lens.makernotes.LensID & 0x10))
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_mFT;
    break;
  case 0x0202:
    if ((!imgdata.lens.LensSerial[0]))
      stmread(imgdata.lens.LensSerial, len, ifp);
    break;
  case 0x0203:
    stmread(imgdata.lens.makernotes.Lens, len, ifp);
    break;
  case 0x0205:
    imgdata.lens.makernotes.MaxAp4MinFocal = libraw_powf64l(sqrt(2.0f), get2() / 256.0f);
    break;
  case 0x0206:
    imgdata.lens.makernotes.MaxAp4MaxFocal = libraw_powf64l(sqrt(2.0f), get2() / 256.0f);
    break;
  case 0x0207:
    imgdata.lens.makernotes.MinFocal = (float)get2();
    break;
  case 0x0208:
    imgdata.lens.makernotes.MaxFocal = (float)get2();
    if (imgdata.lens.makernotes.MaxFocal > 1000.0f)
      imgdata.lens.makernotes.MaxFocal = imgdata.lens.makernotes.MinFocal;
    break;
  case 0x020a:
    imgdata.lens.makernotes.MaxAp4CurFocal = libraw_powf64l(sqrt(2.0f), get2() / 256.0f);
    break;
  case 0x0301:
    imgdata.lens.makernotes.TeleconverterID = fgetc(ifp) << 8;
    fgetc(ifp);
    imgdata.lens.makernotes.TeleconverterID = imgdata.lens.makernotes.TeleconverterID | fgetc(ifp);
    break;
  case 0x0303:
    stmread(imgdata.lens.makernotes.Teleconverter, len, ifp);
    break;
  case 0x0403:
    stmread(imgdata.lens.makernotes.Attachment, len, ifp);
    break;
  }

  return;
}