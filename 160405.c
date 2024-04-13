void LibRaw::parse_fuji(int offset)
{
  unsigned entries, tag, len, save, c;
  ushort raw_inset_present = 0;

  fseek(ifp, offset, SEEK_SET);
  entries = get4();
  if (entries > 255)
    return;
  imgdata.process_warnings |= LIBRAW_WARN_PARSEFUJI_PROCESSED;
  while (entries--)
  {
    tag = get2();
    len = get2();
    save = ftell(ifp);

    if (tag == 0x0100)
    { // RawImageFullSize
      raw_height = get2();
      raw_width = get2();
      raw_inset_present = 1;
    }
    else if (tag == 0x0121)
    { // RawImageSize
      height = get2();
      if ((width = get2()) == 4284)
        width += 3;
    }
    else if (tag == 0x0130)
    { // FujiLayout
      fuji_layout = fgetc(ifp) >> 7;
      fuji_width = !(fgetc(ifp) & 8);
    }
    else if (tag == 0x0131)
    { // XTransLayout
      filters = 9;
      FORC(36)
      {
        int q = fgetc(ifp);
        xtrans_abs[0][35 - c] = MAX(0, MIN(q, 2)); /* & 3;*/
      }
    }
    else if (tag == 0x2ff0)
    { // WB_GRGBLevels
      FORC4 cam_mul[c ^ 1] = get2();
    }

    else if ((tag == 0x0110) && raw_inset_present)
    { // RawImageCropTopLeft
      imgdata.sizes.raw_inset_crop.ctop = get2();
      imgdata.sizes.raw_inset_crop.cleft = get2();
    }
    else if ((tag == 0x0111) && raw_inset_present)
    { // RawImageCroppedSize
      imgdata.sizes.raw_inset_crop.cheight = get2();
      imgdata.sizes.raw_inset_crop.cwidth = get2();
    }
    else if ((tag == 0x0115) && raw_inset_present)
    { // RawImageAspectRatio
      int a = get2();
      int b = get2();
      if (a * b == 6)
        imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_3to2;
      else if (a * b == 12)
        imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_4to3;
      else if (a * b == 144)
        imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_16to9;
      else if (a * b == 1)
        imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_1to1;
    }
    else if (tag == 0x9200)
    { // RelativeExposure
      int a = get4();
      if ((a == 0x01000100) || (a <= 0))
        imFuji.BrightnessCompensation = 0.0f;
      else if (a == 0x00100100)
        imFuji.BrightnessCompensation = 4.0f;
      else
        imFuji.BrightnessCompensation =
            24.0f - float(log((double)a) / log(2.0));
    }
    else if (tag == 0x9650)
    { // RawExposureBias
      short a = (short)get2();
      float b = fMAX(1.0f, get2());
      imFuji.ExpoMidPointShift = a / b;
    }
    else if (tag == 0x2f00)
    { // WB_GRGBLevels
      int nWBs = get4();
      nWBs = MIN(nWBs, 6);
      for (int wb_ind = 0; wb_ind < nWBs; wb_ind++)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + wb_ind][c ^ 1] =
            get2();
        fseek(ifp, 8, SEEK_CUR);
      }
    }
    else if (tag == 0x2000)
    { // WB_GRGBLevelsAuto
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ 1] = get2();
    }
    else if (tag == 0x2100)
    { // WB_GRGBLevelsDaylight
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FineWeather][c ^ 1] = get2();
    }
    else if (tag == 0x2200)
    { // WB_GRGBLevelsCloudy
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Shade][c ^ 1] = get2();
    }
    else if (tag == 0x2300)
    { // WB_GRGBLevelsDaylightFluor
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_D][c ^ 1] = get2();
    }
    else if (tag == 0x2301)
    { // WB_GRGBLevelsDayWhiteFluor
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_N][c ^ 1] = get2();
    }
    else if (tag == 0x2302)
    { // WB_GRGBLevelsWhiteFluorescent
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_WW][c ^ 1] = get2();
    }
    else if (tag == 0x2310)
    { // WB_GRGBLevelsWarmWhiteFluor
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_L][c ^ 1] = get2();
    }
    else if (tag == 0x2311)
    { // WB_GRGBLevelsLivingRoomWarmWhiteFluor
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_W][c ^ 1] = get2();
    }
    else if (tag == 0x2400)
    { // WB_GRGBLevelsTungsten
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][c ^ 1] = get2();
    }
    else if (tag == 0x2410)
    {
      FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][c ^ 1] = get2();
    }

    else if (tag == 0xc000)
    { // RAFData
      c = order;
      order = 0x4949;
      if (len > 20000)
      {
        tag = get4();
        if (tag > 10000)
        {
          imFuji.RAFDataVersion = tag >> 16;
          if (!imFuji.RAFDataVersion)
            imFuji.RAFDataVersion = tag;
          tag = get4();
        }
        if (tag > 10000)
        { // if it is >1000 it normally contains 0x53545257, "WRTS"
          tag = get4();
        }
        width = tag;
        height = get4();
        if (width > raw_width)
          width = raw_width;
        if (height > raw_height)
          height = raw_height;
      }
      if (len == 4096)
      { /* X-A3, X-A5, X-A7, X-A10, X-A20, X-T100, XF10 */
        int wb[4];
        int nWB, tWB, pWB;
        int iCCT = 0;
        is_4K_RAFdata = 1;
        fseek(ifp, save + 0x200, SEEK_SET);
        for (int wb_ind = 0; wb_ind < 42; wb_ind++)
        {
          nWB = get4();
          tWB = get4();
          wb[0] = get4() << 1;
          wb[1] = get4();
          wb[3] = get4();
          wb[2] = get4() << 1;
          if (tWB && (iCCT < 255))
          {
            imgdata.color.WBCT_Coeffs[iCCT][0] = tWB;
            FORC4 imgdata.color.WBCT_Coeffs[iCCT][c + 1] = wb[c];
            iCCT++;
          }
          if (nWB != 70)
          {
            for (pWB = 1; pWB < nFuji_wb_list2; pWB += 2)
            {
              if (Fuji_wb_list2[pWB] == nWB)
              {
                FORC4 imgdata.color.WB_Coeffs[Fuji_wb_list2[pWB - 1]][c] =
                    wb[c];
                break;
              }
            }
          }
        }
      }
      else
      {
        libraw_internal_data.unpacker_data.posRAFData = save;
        libraw_internal_data.unpacker_data.lenRAFData = (len >> 1);
      }
      order = c;
    }
    fseek(ifp, save + len, SEEK_SET);
  }
  height <<= fuji_layout;
  width >>= fuji_layout;
}