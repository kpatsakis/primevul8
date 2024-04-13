void CLASS parseFujiMakernotes(unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
  if ((dng_writer == nonDNG) && (tag == 0x0010)) {
    char FujiSerial[sizeof(imgdata.shootinginfo.InternalBodySerial)];
    char *words[4];
    char yy[2], mm[3], dd[3], ystr[16], ynum[16];
    int year, nwords, ynum_len;
    unsigned c;
    stmread(FujiSerial, len, ifp);
    nwords = getwords(FujiSerial, words, 4, sizeof(imgdata.shootinginfo.InternalBodySerial));
    for (int i = 0; i < nwords; i++) {
      mm[2] = dd[2] = 0;
      if (strnlen(words[i], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) < 18) {
        if (i == 0) {
          strncpy(imgdata.shootinginfo.InternalBodySerial, words[0],
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        } else {
          char tbuf[sizeof(imgdata.shootinginfo.InternalBodySerial)];
          snprintf(tbuf, sizeof(tbuf), "%s %s", imgdata.shootinginfo.InternalBodySerial, words[i]);
          strncpy(imgdata.shootinginfo.InternalBodySerial, tbuf,
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        }
      } else {
        strncpy(dd, words[i] + strnlen(words[i], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 14, 2);
        strncpy(mm, words[i] + strnlen(words[i], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 16, 2);
        strncpy(yy, words[i] + strnlen(words[i], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 18, 2);
        year = (yy[0] - '0') * 10 + (yy[1] - '0');
        if (year < 70) year += 2000; else year += 1900;

        ynum_len = (int)strnlen(words[i], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 18;
        strncpy(ynum, words[i], ynum_len);
        ynum[ynum_len] = 0;
        for (int j = 0; ynum[j] && ynum[j + 1] && sscanf(ynum + j, "%2x", &c); j += 2)
          ystr[j / 2] = c;
        ystr[ynum_len / 2 + 1] = 0;
        strcpy(model2, ystr);

        if (i == 0) {
          char tbuf[sizeof(imgdata.shootinginfo.InternalBodySerial)];

          if (nwords == 1) {
            snprintf(tbuf, sizeof(tbuf), "%s %s %d:%s:%s",
                     words[0] + strnlen(words[0], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 12, ystr,
                     year, mm, dd);

          } else {
            snprintf(tbuf, sizeof(tbuf), "%s %d:%s:%s %s", ystr, year, mm, dd,
                     words[0] + strnlen(words[0], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 12);
          }
          strncpy(imgdata.shootinginfo.InternalBodySerial, tbuf,
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);

        } else {
          char tbuf[sizeof(imgdata.shootinginfo.InternalBodySerial)];
          snprintf(tbuf, sizeof(tbuf), "%s %s %d:%s:%s %s", imgdata.shootinginfo.InternalBodySerial, ystr, year, mm,
                   dd, words[i] + strnlen(words[i], sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) - 12);
          strncpy(imgdata.shootinginfo.InternalBodySerial, tbuf,
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        }
      }
    }

  } else switch (tag) {
  case 0x1002:
    imgdata.makernotes.fuji.WB_Preset = get2();
    break;
  case 0x1011:
    imgdata.other.FlashEC = getreal(type);
    break;
  case 0x1020:
    imgdata.makernotes.fuji.Macro = get2();
    break;
  case 0x1021:
    imgdata.makernotes.fuji.FocusMode = imgdata.shootinginfo.FocusMode = get2();
    break;
  case 0x1022:
    imgdata.makernotes.fuji.AFMode = get2();
    break;
  case 0x1023:
    imgdata.makernotes.fuji.FocusPixel[0] = get2();
    imgdata.makernotes.fuji.FocusPixel[1] = get2();
    break;
  case 0x1034:
    imgdata.makernotes.fuji.ExrMode = get2();
    break;
  case 0x104d:
    imgdata.makernotes.fuji.CropMode = get2();
    break;
  case 0x1050:
    imgdata.makernotes.fuji.ShutterType = get2();
    break;
  case 0x1400:
    imgdata.makernotes.fuji.FujiDynamicRange = get2();
    break;
  case 0x1401:
    imgdata.makernotes.fuji.FujiFilmMode = get2();
    break;
  case 0x1402:
    imgdata.makernotes.fuji.FujiDynamicRangeSetting = get2();
    break;
  case 0x1403:
    imgdata.makernotes.fuji.FujiDevelopmentDynamicRange = get2();
    break;
  case 0x140b:
    imgdata.makernotes.fuji.FujiAutoDynamicRange = get2();
    break;
  case 0x1404:
    imgdata.lens.makernotes.MinFocal = getreal(type);
    break;
  case 0x1405:
    imgdata.lens.makernotes.MaxFocal = getreal(type);
    break;
  case 0x1406:
    imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type);
    break;
  case 0x1407:
    imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type);
    break;
  case 0x1422:
    imgdata.makernotes.fuji.ImageStabilization[0] = get2();
    imgdata.makernotes.fuji.ImageStabilization[1] = get2();
    imgdata.makernotes.fuji.ImageStabilization[2] = get2();
    imgdata.shootinginfo.ImageStabilization =
        (imgdata.makernotes.fuji.ImageStabilization[0] << 9) + imgdata.makernotes.fuji.ImageStabilization[1];
    break;
  case 0x1431:
    imgdata.makernotes.fuji.Rating = get4();
    break;
  case 0x3820:
    imgdata.makernotes.fuji.FrameRate = get2();
    break;
  case 0x3821:
    imgdata.makernotes.fuji.FrameWidth = get2();
    break;
  case 0x3822:
    imgdata.makernotes.fuji.FrameHeight = get2();
    break;
  }
  return;
}