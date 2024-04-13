void CLASS parseOlympus_CameraSettings (int base, unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
// uptag 0x2020

  int c;
  uchar uc;
  switch (tag) {
  case 0x0101:
    if (dng_writer == nonDNG) {
      thumb_offset = get4() + base;
    }
    break;
  case 0x0102:
    if (dng_writer == nonDNG) {
      thumb_length = get4();
  }
    break;
  case 0x0200:
    imgdata.shootinginfo.ExposureMode = get2();
    break;
  case 0x0202:
    imgdata.shootinginfo.MeteringMode = get2();
    break;
  case 0x0301:
    imgdata.shootinginfo.FocusMode =
    imgdata.makernotes.olympus.FocusMode[0] = get2();
    if (len == 2) {
      imgdata.makernotes.olympus.FocusMode[1] = get2();
    }
    break;
  case 0x0304:
    for (c = 0; c < 64; c++) {
      imgdata.makernotes.olympus.AFAreas[c] = get4();
    }
    break;
  case 0x0305:
    for (c = 0; c < 5; c++) {
      imgdata.makernotes.olympus.AFPointSelected[c] = getreal(type);
    }
    break;
  case 0x0306:
    fread(&uc, 1, 1, ifp);
    imgdata.makernotes.olympus.AFFineTune = uc;
    break;
  case 0x0307:
    FORC3 imgdata.makernotes.olympus.AFFineTuneAdj[c] = get2();
    break;
  case 0x0401:
    imgdata.other.FlashEC = getreal(type);
    break;
  case 0x0507:
    imgdata.makernotes.olympus.ColorSpace = get2();
    break;
  case 0x0600:
    imgdata.shootinginfo.DriveMode =
    imgdata.makernotes.olympus.DriveMode[0] = get2();
    for (c = 1; c < len; c++) {
      imgdata.makernotes.olympus.DriveMode[c] = get2();
    }
    break;
  case 0x0604:
    imgdata.shootinginfo.ImageStabilization = get4();
    break;
  }

  return;
}