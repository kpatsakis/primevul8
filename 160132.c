void LibRaw::parseFujiMakernotes(unsigned tag, unsigned type, unsigned len,
                                 unsigned dng_writer)
{
  if ((dng_writer == nonDNG) && (tag == 0x0010))
  {
    char FujiSerial[sizeof(imgdata.shootinginfo.InternalBodySerial)];
    char *words[4];
    char yy[2], mm[3], dd[3], ystr[16], ynum[16];
    int year, nwords, ynum_len;
    unsigned c;
    memset(FujiSerial, 0, sizeof(imgdata.shootinginfo.InternalBodySerial));
    ifp->read(FujiSerial, MIN(len,sizeof(FujiSerial)), 1);
    nwords = getwords(FujiSerial, words, 4,
                      sizeof(imgdata.shootinginfo.InternalBodySerial));
    for (int i = 0; i < nwords; i++)
    {
      mm[2] = dd[2] = 0;
      if (strnlen(words[i],
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) < 18)
      {
        if (i == 0)
        {
          strncpy(imgdata.shootinginfo.InternalBodySerial, words[0],
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        }
        else
        {
          char tbuf[sizeof(imgdata.shootinginfo.InternalBodySerial)];
          snprintf(tbuf, sizeof(tbuf), "%s %s",
                   imgdata.shootinginfo.InternalBodySerial, words[i]);
          strncpy(imgdata.shootinginfo.InternalBodySerial, tbuf,
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        }
      }
      else
      {
        strncpy(
            dd,
            words[i] +
                strnlen(words[i],
                        sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) -
                14,
            2);
        strncpy(
            mm,
            words[i] +
                strnlen(words[i],
                        sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) -
                16,
            2);
        strncpy(
            yy,
            words[i] +
                strnlen(words[i],
                        sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) -
                18,
            2);
        year = (yy[0] - '0') * 10 + (yy[1] - '0');
        if (year < 70)
          year += 2000;
        else
          year += 1900;

        ynum_len = MIN(
            (sizeof(ynum) - 1),
            (int)strnlen(words[i],
                         sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) -
                18);
        strncpy(ynum, words[i], ynum_len);
        ynum[ynum_len] = 0;
        for (int j = 0; ynum[j] && ynum[j + 1] && sscanf(ynum + j, "%2x", &c);
             j += 2)
          ystr[j / 2] = c;
        ystr[ynum_len / 2 + 1] = 0;
        strcpy(model2, ystr);

        if (i == 0)
        {
          char tbuf[sizeof(imgdata.shootinginfo.InternalBodySerial)];

          if (nwords == 1)
          {
            snprintf(
                tbuf, sizeof(tbuf), "%s %s %d:%s:%s",
                words[0] +
                    strnlen(words[0],
                            sizeof(imgdata.shootinginfo.InternalBodySerial) -
                                1) -
                    12,
                ystr, year, mm, dd);
          }
          else
          {
            snprintf(
                tbuf, sizeof(tbuf), "%s %d:%s:%s %s", ystr, year, mm, dd,
                words[0] +
                    strnlen(words[0],
                            sizeof(imgdata.shootinginfo.InternalBodySerial) -
                                1) -
                    12);
          }
          strncpy(imgdata.shootinginfo.InternalBodySerial, tbuf,
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        }
        else
        {
          char tbuf[sizeof(imgdata.shootinginfo.InternalBodySerial)];
          snprintf(
              tbuf, sizeof(tbuf), "%s %s %d:%s:%s %s",
              imgdata.shootinginfo.InternalBodySerial, ystr, year, mm, dd,
              words[i] +
                  strnlen(words[i],
                          sizeof(imgdata.shootinginfo.InternalBodySerial) - 1) -
                  12);
          strncpy(imgdata.shootinginfo.InternalBodySerial, tbuf,
                  sizeof(imgdata.shootinginfo.InternalBodySerial) - 1);
        }
      }
    }
  }
  else
    switch (tag)
    {
    case 0x1002:
      imFuji.WB_Preset = get2();
      break;
    case 0x1011:
      imgdata.makernotes.common.FlashEC = getreal(type);
      break;
    case 0x1020:
      imFuji.Macro = get2();
      break;
    case 0x1021:
      imFuji.FocusMode = imgdata.shootinginfo.FocusMode = get2();
      break;
    case 0x1022:
      imFuji.AFMode = get2();
      break;
    case 0x1023:
      imFuji.FocusPixel[0] = get2();
      imFuji.FocusPixel[1] = get2();
      break;
    case 0x1034:
      imFuji.ExrMode = get2();
      break;
    case 0x104d:
      FujiCropMode = get2(); // odd: one of raw dimensions here can be lost
      break;
    case 0x1050:
      imFuji.ShutterType = get2();
      break;
    case 0x1103:
      imgdata.shootinginfo.DriveMode = get2();
      imFuji.DriveMode = imgdata.shootinginfo.DriveMode & 0xff;
      break;
    case 0x1400:
      imFuji.DynamicRange = get2();
      break;
    case 0x1401:
      imFuji.FilmMode = get2();
      break;
    case 0x1402:
      imFuji.DynamicRangeSetting = get2();
      break;
    case 0x1403:
      imFuji.DevelopmentDynamicRange = get2();
      break;
    case 0x140b:
      imFuji.AutoDynamicRange = get2();
      break;
    case 0x1404:
      ilm.MinFocal = getreal(type);
      break;
    case 0x1405:
      ilm.MaxFocal = getreal(type);
      break;
    case 0x1406:
      ilm.MaxAp4MinFocal = getreal(type);
      break;
    case 0x1407:
      ilm.MaxAp4MaxFocal = getreal(type);
      break;
    case 0x1422:
      imFuji.ImageStabilization[0] = get2();
      imFuji.ImageStabilization[1] = get2();
      imFuji.ImageStabilization[2] = get2();
      imgdata.shootinginfo.ImageStabilization =
          (imFuji.ImageStabilization[0] << 9) + imFuji.ImageStabilization[1];
      break;
    case 0x1431:
      imFuji.Rating = get4();
      break;
    case 0x3820:
      imFuji.FrameRate = get2();
      break;
    case 0x3821:
      imFuji.FrameWidth = get2();
      break;
    case 0x3822:
      imFuji.FrameHeight = get2();
      break;
    }
  return;
}