void LibRaw::parseCanonMakernotes(unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
  int c;
  unsigned i;

  if (tag == 0x0001) {
    Canon_CameraSettings(len);

  } else if (tag == 0x0002) { // focal length
    ilm.FocalType = get2();
    ilm.CurFocal = get2();
    if (ilm.FocalUnits > 1) {
      ilm.CurFocal /= (float)ilm.FocalUnits;
    }

  } else if (tag == 0x0004) { // subdir, ShotInfo
    short tempAp;
    if (dng_writer == nonDNG) {
      if ((i = (get4(), get2())) != 0x7fff &&
          (!iso_speed || iso_speed == 65535)) {
        iso_speed = 50 * libraw_powf64l(2.0, i / 32.0 - 4);
      }
      get4();
      if (((i = get2()) != 0xffff) && !shutter) {
        shutter = libraw_powf64l(2.0, (short)i / -32.0);
      }
      imCanon.wbi = (get2(), get2());
      shot_order = (get2(), get2());
      fseek(ifp, 4, SEEK_CUR);
    } else
      fseek(ifp, 24, SEEK_CUR);
    tempAp = get2();
    if (tempAp != 0)
      imgdata.makernotes.common.CameraTemperature = (float)(tempAp - 128);
    tempAp = get2();
    if (tempAp != -1)
      imgdata.makernotes.common.FlashGN = ((float)tempAp) / 32;
    get2();

    imgdata.makernotes.common.FlashEC = _CanonConvertEV((signed short)get2());
    fseek(ifp, 8 - 32, SEEK_CUR);
    if ((tempAp = get2()) != 0x7fff)
      ilm.CurAp = _CanonConvertAperture(tempAp);
    if (ilm.CurAp < 0.7f) {
      fseek(ifp, 32, SEEK_CUR);
      ilm.CurAp = _CanonConvertAperture(get2());
    }
    if (!aperture)
      aperture = ilm.CurAp;

  } else if ((tag == 0x0007) && (dng_writer == nonDNG)) {
    fgets(model2, 64, ifp);

  } else if ((tag == 0x0008) && (dng_writer == nonDNG)) {
    shot_order = get4();

  } else if ((tag == 0x0009)  && (dng_writer == nonDNG)) {
    fread(artist, 64, 1, ifp);

  } else if (tag == 0x000c) {
    unsigned tS = get4();
    sprintf(imgdata.shootinginfo.BodySerial, "%d", tS);

  } else if ((tag == 0x0029) && (dng_writer == nonDNG)) { // PowerShot G9
      c = imCanon.wbi < 18 ? "012347800000005896"[imCanon.wbi] - '0' : 0;
      fseek(ifp, 8 + c * 32, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get4();

  } else if ((tag == 0x0081) && (dng_writer == nonDNG)) { // EOS-1D, EOS-1DS
    data_offset = get4();
    fseek(ifp, data_offset + 41, SEEK_SET);
    raw_height = get2() * 2;
    raw_width = get2();
    filters = 0x61616161;

  } else if (tag == 0x0095 && !ilm.Lens[0])
  { // lens model tag
    fread(ilm.Lens, 64, 1, ifp);
    if (!strncmp(ilm.Lens, "EF-S", 4))
    {
      memmove(ilm.Lens + 5, ilm.Lens + 4, 60);
      ilm.Lens[4] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 4);
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF_S;
      ilm.LensFormat = LIBRAW_FORMAT_APSC;
    }
    else if (!strncmp(ilm.Lens, "EF-M", 4))
    {
      memmove(ilm.Lens + 5, ilm.Lens + 4, 60);
      ilm.Lens[4] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 4);
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF_M;
      ilm.LensFormat = LIBRAW_FORMAT_APSC;
    }
    else if (!strncmp(ilm.Lens, "EF", 2))
    {
      memmove(ilm.Lens + 3, ilm.Lens + 2, 62);
      ilm.Lens[2] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 2);
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
      ilm.LensFormat = LIBRAW_FORMAT_FF;
    }
    else if (!strncmp(ilm.Lens, "CN-E", 4))
    {
      memmove(ilm.Lens + 5, ilm.Lens + 4, 60);
      ilm.Lens[4] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 4);
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
      ilm.LensFormat = LIBRAW_FORMAT_FF;
    }
    else if (!strncmp(ilm.Lens, "TS-E", 4))
    {
      memmove(ilm.Lens + 5, ilm.Lens + 4, 60);
      ilm.Lens[4] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 4);
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
      ilm.LensFormat = LIBRAW_FORMAT_FF;
    }
    else if (!strncmp(ilm.Lens, "MP-E", 4))
    {
      memmove(ilm.Lens + 5, ilm.Lens + 4, 60);
      ilm.Lens[4] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 4);
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
      ilm.LensFormat = LIBRAW_FORMAT_FF;
    }
    else if (!strncmp(ilm.Lens, "RF", 2))
    {
      memmove(ilm.Lens + 3, ilm.Lens + 2, 62);
      ilm.Lens[2] = ' ';
      memcpy(ilm.LensFeatures_pre, ilm.Lens, 2);
      ilm.LensMount = LIBRAW_MOUNT_Canon_RF;
      ilm.LensFormat = LIBRAW_FORMAT_FF;
    }
  }
  else if (tag == 0x009a)
  { // AspectInfo
    i = get4();
    switch (i)
    {
    case 0:
    case 12: /* APS-H crop */
    case 13: /* APS-C crop */
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_3to2;
      break;
    case 1:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_1to1;
      break;
    case 2:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_4to3;
      break;
    case 7:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_16to9;
      break;
    case 8:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_5to4;
      break;
    default:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_OTHER;
      break;
    }
    imgdata.sizes.raw_inset_crop.cwidth = get4();
    imgdata.sizes.raw_inset_crop.cheight = get4();
    imgdata.sizes.raw_inset_crop.cleft = get4();
    imgdata.sizes.raw_inset_crop.ctop = get4();

  } else if ((tag == 0x00a4) && (dng_writer == nonDNG)) { // EOS-1D, EOS-1DS
    fseek(ifp, imCanon.wbi * 48, SEEK_CUR);
    FORC3 cam_mul[c] = get2();

  } else if (tag == 0x00a9) {
    long int save1 = ftell(ifp);
    fseek(ifp, (0x1 << 1), SEEK_CUR);
    FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
    Canon_WBpresets(0, 0);
    fseek(ifp, save1, SEEK_SET);
  }
  else if (tag == 0x00e0)
  { // SensorInfo
    imCanon.SensorWidth = (get2(), get2());
    imCanon.SensorHeight = get2();
    imCanon.SensorLeftBorder = (get2(), get2(), get2());
    imCanon.SensorTopBorder = get2();
    imCanon.SensorRightBorder = get2();
    imCanon.SensorBottomBorder = get2();
    imCanon.BlackMaskLeftBorder = get2();
    imCanon.BlackMaskTopBorder = get2();
    imCanon.BlackMaskRightBorder = get2();
    imCanon.BlackMaskBottomBorder = get2();
  }
  else if (tag == 0x4001 && len > 500)
  {
    int bls = 0;
    long int offsetChannelBlackLevel = 0L;
    long int offsetChannelBlackLevel2 = 0L;
    long int offsetWhiteLevels = 0L;
    long int save1 = ftell(ifp);

    switch (len)
    {

    case 582:
      imCanon.ColorDataVer = 1; // 20D / 350D

      fseek(ifp, save1 + (0x0019 << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x001e << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0041 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0046 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom2][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0023 << 1), SEEK_SET);
      Canon_WBpresets(2, 2);
      fseek(ifp, save1 + (0x004b << 1), SEEK_SET);
      Canon_WBCTpresets(1); // ABCT
      offsetChannelBlackLevel = save1 + (0x00a6 << 1);
      break;

    case 653:
      imCanon.ColorDataVer = 2; // 1Dmk2 / 1DsMK2

      fseek(ifp, save1 + (0x0018 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0022 << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x0090 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0095 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom2][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x009a << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom3][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0027 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00a4 << 1), SEEK_SET);
      Canon_WBCTpresets(1); // ABCT
      offsetChannelBlackLevel = save1 + (0x011e << 1);
      break;

    case 796:
      imCanon.ColorDataVer = 3; // 1DmkIIN / 5D / 30D / 400D
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x003f << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0071 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0076 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom2][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x007b << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom3][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0080 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x004e << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x0085 << 1), SEEK_SET);
      Canon_WBCTpresets(0); // BCAT
      offsetChannelBlackLevel = save1 + (0x00c4 << 1);
      break;

    // 1DmkIII / 1DSmkIII / 1DmkIV / 5DmkII
    // 7D / 40D / 50D / 60D / 450D / 500D
    // 550D / 1000D / 1100D
    case 674:
    case 692:
    case 702:
    case 1227:
    case 1250:
    case 1251:
    case 1337:
    case 1338:
    case 1346:
      imCanon.ColorDataVer = 4;
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x003f << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x004e << 1), SEEK_SET);
      FORC4 sraw_mul[c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0053 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00a8 << 1), SEEK_SET);
      Canon_WBCTpresets(0); // BCAT

      if ((imCanon.ColorDataSubVer == 4) ||
          (imCanon.ColorDataSubVer == 5))
      {
        offsetChannelBlackLevel = save1 + (0x02b4 << 1);
        offsetWhiteLevels = save1 + (0x02b8 << 1);
      }
      else if ((imCanon.ColorDataSubVer == 6) ||
               (imCanon.ColorDataSubVer == 7))
      {
        offsetChannelBlackLevel = save1 + (0x02cb << 1);
        offsetWhiteLevels = save1 + (0x02cf << 1);
      }
      else if (imCanon.ColorDataSubVer == 9)
      {
        offsetChannelBlackLevel = save1 + (0x02cf << 1);
        offsetWhiteLevels = save1 + (0x02d3 << 1);
      }
      else
        offsetChannelBlackLevel = save1 + (0x00e7 << 1);
      break;

    case 5120:  // PowerShot G10, G12, G5 X, G7 X, G9 X, EOS M3, EOS M5, EOS M6
      imCanon.ColorDataVer = 5;
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0047 << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();

      if (imCanon.ColorDataSubVer == 0xfffc)
      { // -4: G7 X Mark II, G9 X Mark II, G1 X Mark III, M5, M100, M6
        fseek(ifp, save1 + (0x004f << 1), SEEK_SET);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
        fseek(ifp, 8, SEEK_CUR);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] =
            get2();
        fseek(ifp, 8, SEEK_CUR);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Other][c ^ (c >> 1)] = get2();
        fseek(ifp, 8, SEEK_CUR);
        Canon_WBpresets(8, 24);
        fseek(ifp, 168, SEEK_CUR);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_WW][c ^ (c >> 1)] = get2();
        fseek(ifp, 24, SEEK_CUR);
        Canon_WBCTpresets(2); // BCADT
        offsetChannelBlackLevel = save1 + (0x014d << 1);
        offsetWhiteLevels = save1 + (0x0569 << 1);
      }
      else if (imCanon.ColorDataSubVer == 0xfffd)
      { // -3: M10/M3/G1 X/G1 X II/G10/G11/G12/G15/G16/G3 X/G5 X/G7 X/G9
        // X/S100/S110/S120/S90/S95/SX1 IX/SX50 HS/SX60 HS
        fseek(ifp, save1 + (0x004c << 1), SEEK_SET);
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
        get2();
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] =
            get2();
        get2();
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Other][c ^ (c >> 1)] = get2();
        get2();
        Canon_WBpresets(2, 12);
        fseek(ifp, save1 + (0x00ba << 1), SEEK_SET);
        Canon_WBCTpresets(2); // BCADT
        offsetChannelBlackLevel = save1 + (0x0108 << 1);
      }
      break;

    case 1273:
    case 1275:
      imCanon.ColorDataVer = 6; // 600D / 1200D
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x003f << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0062 << 1), SEEK_SET);
      FORC4 sraw_mul[c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0067 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00bc << 1), SEEK_SET);
      Canon_WBCTpresets(0); // BCAT
      offsetChannelBlackLevel = save1 + (0x01df << 1);
      offsetWhiteLevels = save1 + (0x01e3 << 1);
      break;

    // 1DX / 5DmkIII / 6D / 100D / 650D / 700D / EOS M / 7DmkII / 750D / 760D
    case 1312:
    case 1313:
    case 1316:
    case 1506:
      imCanon.ColorDataVer = 7;
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x003f << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x007b << 1), SEEK_SET);
      FORC4 sraw_mul[c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0080 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x00d5 << 1), SEEK_SET);
      Canon_WBCTpresets(0); // BCAT

      if (imCanon.ColorDataSubVer == 10)
      {
        offsetChannelBlackLevel = save1 + (0x01f8 << 1);
        offsetWhiteLevels = save1 + (0x01fc << 1);
      }
      else if (imCanon.ColorDataSubVer == 11)
      {
        offsetChannelBlackLevel = save1 + (0x02d8 << 1);
        offsetWhiteLevels = save1 + (0x02dc << 1);
      }
      break;

    // 5DS / 5DS R / 80D / 1300D / 1500D / 3000D / 5D4 / 800D / 77D / 6D II /
    // 200D
    case 1560:
    case 1592:
    case 1353:
    case 1602:
      imCanon.ColorDataVer = 8;
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x003f << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      fseek(ifp, save1 + (0x0044 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0049 << 1), SEEK_SET);
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();

      fseek(ifp, save1 + (0x0080 << 1), SEEK_SET);
      FORC4 sraw_mul[c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0085 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x0107 << 1), SEEK_SET);
      Canon_WBCTpresets(0); // BCAT

      if (imCanon.ColorDataSubVer == 14)
      { // 1300D / 1500D / 3000D
        offsetChannelBlackLevel = save1 + (0x022c << 1);
        offsetWhiteLevels = save1 + (0x0230 << 1);
      }
      else
      {
        offsetChannelBlackLevel = save1 + (0x030a << 1);
        offsetWhiteLevels = save1 + (0x030e << 1);
      }
      break;

    case 1820: // M50, ColorDataSubVer 16
    case 1824: // EOS R, SX740HS, ColorDataSubVer 17
    case 1816: // EOS RP, SX70HS, ColorDataSubVer 18;
               // EOS M6 Mark II, EOS 90D, G7XmkIII, ColorDataSubVer 19
      imCanon.ColorDataVer = 9;
      imCanon.ColorDataSubVer = get2();

      fseek(ifp, save1 + (0x0047 << 1), SEEK_SET);
      FORC4 cam_mul[c ^ (c >> 1)] = (float)get2();
      get2();
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ (c >> 1)] = get2();
      get2();
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Measured][c ^ (c >> 1)] = get2();
      fseek(ifp, save1 + (0x0088 << 1), SEEK_SET);
      Canon_WBpresets(2, 12);
      fseek(ifp, save1 + (0x010a << 1), SEEK_SET);
      Canon_WBCTpresets(0);
      offsetChannelBlackLevel = save1 + (0x0318 << 1);
      offsetChannelBlackLevel2 = save1 + (0x0149 << 1);
      offsetWhiteLevels = save1 + (0x031c << 1);
      break;
    }

    if (offsetChannelBlackLevel)
    {
      fseek(ifp, offsetChannelBlackLevel, SEEK_SET);
      FORC4
        bls += (imCanon.ChannelBlackLevel[c ^ (c >> 1)] = get2());
      imCanon.AverageBlackLevel = bls / 4;
    }
    if (offsetWhiteLevels)
    {
      if ((offsetWhiteLevels - offsetChannelBlackLevel) != 8L)
        fseek(ifp, offsetWhiteLevels, SEEK_SET);
      imCanon.NormalWhiteLevel = get2();
      imCanon.SpecularWhiteLevel = get2();
      FORC4
        imgdata.color.linear_max[c] = imCanon.SpecularWhiteLevel;
    }

    if(!imCanon.AverageBlackLevel && offsetChannelBlackLevel2)
    {
        fseek(ifp, offsetChannelBlackLevel2, SEEK_SET);
        FORC4
            bls += (imCanon.ChannelBlackLevel[c ^ (c >> 1)] = get2());
        imCanon.AverageBlackLevel = bls / 4;
    }
    fseek(ifp, save1, SEEK_SET);

  } else if (tag == 0x4013) {
    get4();
    imCanon.AFMicroAdjMode = get4();
    float a = get4();
    float b = get4();
    if (fabsf(b) > 0.001f)
      imCanon.AFMicroAdjValue = a / b;

  } else if ((tag == 0x4021) && (dng_writer == nonDNG) &&
             (imCanon.multishot[0] = get4()) &&
             (imCanon.multishot[1] = get4())) {
    if (len >= 4) {
      imCanon.multishot[2] = get4();
      imCanon.multishot[3] = get4();
    }
    FORC4 cam_mul[c] = 1024;
  }

}