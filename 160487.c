void LibRaw::parseAdobeRAFMakernote()
{

  uchar *PrivateMknBuf;
  unsigned posPrivateMknBuf;
  unsigned PrivateMknLength;
  unsigned PrivateOrder;
  unsigned PrivateEntries, PrivateTagID;
  unsigned PrivateTagBytes;
  unsigned wb_section_offset = 0;
  int posWB;
  int c;

#define CHECKSPACE(s)                                                          \
  if (posPrivateMknBuf + (s) > PrivateMknLength)                               \
  {                                                                            \
    free(PrivateMknBuf);                                                       \
    return;                                                                    \
  }
#define isWB(posWB)                                                            \
  sget2(posWB) != 0 && sget2(posWB + 2) != 0 && sget2(posWB + 4) != 0 &&       \
      sget2(posWB + 6) != 0 && sget2(posWB + 8) != 0 &&                        \
      sget2(posWB + 10) != 0 && sget2(posWB) != 0xff &&                        \
      sget2(posWB + 2) != 0xff && sget2(posWB + 4) != 0xff &&                  \
      sget2(posWB + 6) != 0xff && sget2(posWB + 8) != 0xff &&                  \
      sget2(posWB + 10) != 0xff && sget2(posWB) == sget2(posWB + 6) &&         \
      sget2(posWB) < sget2(posWB + 2) && sget2(posWB) < sget2(posWB + 4) &&    \
      sget2(posWB) < sget2(posWB + 8) && sget2(posWB) < sget2(posWB + 10)
#define icWBC imgdata.color.WB_Coeffs
#define imfRAFDataVersion imFuji.RAFDataVersion
#define imfRAFVersion imFuji.RAFVersion

  order = 0x4d4d;
  PrivateMknLength = get4();

  if ((PrivateMknLength > 4) && (PrivateMknLength < 10240000) &&
      (PrivateMknBuf = (uchar *)malloc(PrivateMknLength + 1024)))
  { // 1024b for safety
    fread(PrivateMknBuf, PrivateMknLength, 1, ifp);
    PrivateOrder = sget2(PrivateMknBuf);
    posPrivateMknBuf = sget4(PrivateMknBuf + 2) + 12;

    memcpy(imFuji.SerialSignature, PrivateMknBuf + 6, 0x0c);
    imFuji.SerialSignature[0x0c] = 0;
    memcpy(model, PrivateMknBuf + 0x12, 0x20);
    model[0x20] = 0;
    memcpy(model2, PrivateMknBuf + 0x32, 4);
    model2[4] = 0;
    strcpy(imFuji.RAFVersion, model2);
    PrivateEntries = sget2(PrivateMknBuf + posPrivateMknBuf);
    if ((PrivateEntries > 1000) ||
        ((PrivateOrder != 0x4d4d) && (PrivateOrder != 0x4949)))
    {
      free(PrivateMknBuf);
      return;
    }
    posPrivateMknBuf += 2;
    /*
     * because Adobe DNG converter strips or misplaces 0xfnnn tags,
     * Auto WB for following cameras is missing for now
     * - F550EXR
     * - F600EXR
     * - F770EXR
     * - F800EXR
     * - F900EXR
     * - HS10
     * - HS11
     * - HS20EXR
     * - HS30EXR
     * - HS50EXR
     * - S1
     * - SL1000
     **/
    while (PrivateEntries--)
    {
      order = 0x4d4d;
      CHECKSPACE(4);
      PrivateTagID = sget2(PrivateMknBuf + posPrivateMknBuf);
      PrivateTagBytes = sget2(PrivateMknBuf + posPrivateMknBuf + 2);
      posPrivateMknBuf += 4;
      order = PrivateOrder;

      if (PrivateTagID == 0x2000)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2100)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FineWeather][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2200)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Shade][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2300)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_D][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2301)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_N][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2302)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_WW][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2310)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_L][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2311)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_W][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2400)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2410)
      {
        FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }
      else if (PrivateTagID == 0x2f00)
      {
        int nWBs = MIN(sget4(PrivateMknBuf + posPrivateMknBuf), 6);
        posWB = posPrivateMknBuf + 4;
        for (int wb_ind = 0; wb_ind < nWBs; wb_ind++)
        {
          FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + wb_ind][c ^ 1] =
              sget2(PrivateMknBuf + posWB + (c << 1));
          posWB += 8;
        }
      }
      else if (PrivateTagID == 0x2ff0)
      {
        FORC4 cam_mul[c ^ 1] =
            sget2(PrivateMknBuf + posPrivateMknBuf + (c << 1));
      }

      else if (PrivateTagID == 0x9650)
      {
        CHECKSPACE(4);
        short a = (short)sget2(PrivateMknBuf + posPrivateMknBuf);
        float b = fMAX(1.0f, sget2(PrivateMknBuf + posPrivateMknBuf + 2));
        imFuji.ExpoMidPointShift = a / b;
      }
      else if ((PrivateTagID == 0xc000) && (PrivateTagBytes > 3) &&
               (PrivateTagBytes < 10240000))
      {
        order = 0x4949;
        if (PrivateTagBytes != 4096)
        { // not one of Fuji X-A3, X-A5, X-A7, X-A10, X-A20, X-T100, XF10
          if (!sget2(PrivateMknBuf + posPrivateMknBuf))
            imfRAFDataVersion = sget2(PrivateMknBuf + posPrivateMknBuf + 2);

          for (posWB = 0; posWB < PrivateTagBytes - 16; posWB++)
          {
            if ((!memcmp(PrivateMknBuf + posWB, "TSNERDTS", 8) &&
                 (sget2(PrivateMknBuf + posWB + 10) > 125)))
            {
              posWB += 10;
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][1] =
                  imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][3] =
                      sget2(PrivateMknBuf + posWB);
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][0] =
                  sget2(PrivateMknBuf + posWB + 2);
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][2] =
                  sget2(PrivateMknBuf + posWB + 4);
              break;
            }
          }

          if (imfRAFDataVersion == 0x0146 || // X20
              imfRAFDataVersion == 0x0149 || // X100S
              imfRAFDataVersion == 0x0249)
          { // X100S
            wb_section_offset = 0x1410;
          }
          else if (imfRAFDataVersion == 0x014d || // X-M1
                   imfRAFDataVersion == 0x014e)
          { // X-A1, X-A2
            wb_section_offset = 0x1474;
          }
          else if (imfRAFDataVersion == 0x014f || // X-E2
                   imfRAFDataVersion == 0x024f || // X-E2
                   imfRAFDataVersion == 0x025d)
          { // X-H1
            wb_section_offset = 0x1480;
          }
          else if (imfRAFDataVersion == 0x0150)
          { // XQ1, XQ2
            wb_section_offset = 0x1414;
          }
          else if (imfRAFDataVersion == 0x0151 || // X-T1 w/diff. fws
                   imfRAFDataVersion == 0x0251 || imfRAFDataVersion == 0x0351 ||
                   imfRAFDataVersion == 0x0451 || imfRAFDataVersion == 0x0551)
          {
            wb_section_offset = 0x14b0;
          }
          else if (imfRAFDataVersion == 0x0152 || // X30
                   imfRAFDataVersion == 0x0153)
          { // X100T
            wb_section_offset = 0x1444;
          }
          else if (imfRAFDataVersion == 0x0154)
          { // X-T10
            wb_section_offset = 0x1824;
          }
          else if (imfRAFDataVersion == 0x0155)
          { // X70
            wb_section_offset = 0x17b4;
          }
          else if (imfRAFDataVersion == 0x0255)
          { // X-Pro2
            wb_section_offset = 0x135c;
          }
          else if (imfRAFDataVersion == 0x0258 || // X-T2
                   imfRAFDataVersion == 0x025b)
          { // X-T20
            wb_section_offset = 0x13dc;
          }
          else if (imfRAFDataVersion == 0x0259)
          { // X100F
            wb_section_offset = 0x1370;
          }
          else if (imfRAFDataVersion == 0x025a)
          { // GFX 50S
            wb_section_offset = 0x1424;
          }
          else if (imfRAFDataVersion == 0x025c)
          { // X-E3
            wb_section_offset = 0x141c;
          }
          else if (imfRAFDataVersion == 0x025e)
          { // X-T3
            wb_section_offset = 0x2014;
          }
          else if (imfRAFDataVersion == 0x025f)
          { // X-T30, GFX 50R, GFX 100
            if (!strcmp(model, "X-T30"))
              wb_section_offset = 0x20b8;
            else if (!strcmp(model, "GFX 50R"))
              wb_section_offset = 0x1424;
            else if (!strcmp(model, "GFX 100"))
              wb_section_offset = 0x20e4;
          }
          else if (imfRAFDataVersion == 0x0355)
          { // X-E2S
            wb_section_offset = 0x1840;

            /* try for unknown RAF Data versions */
          }
          else if (!strcmp(model, "X20"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1410))
              wb_section_offset = 0x1410;
          }
          else if (!strcmp(model, "X100S"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1410))
              wb_section_offset = 0x1410;
          }
          else if (!strcmp(model, "X-M1"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1474))
              wb_section_offset = 0x1474;
          }
          else if (!strcmp(model, "X-A1"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1474))
              wb_section_offset = 0x1474;
          }
          else if (!strcmp(model, "X-A2"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1474))
              wb_section_offset = 0x1474;
          }
          else if (!strcmp(model, "X-E2"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1480))
              wb_section_offset = 0x1480;
          }
          else if (!strcmp(model, "X-H1"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1480))
              wb_section_offset = 0x1480;
          }
          else if (!strcmp(model, "XQ1"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1414))
              wb_section_offset = 0x1414;
          }
          else if (!strcmp(model, "XQ2"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1414))
              wb_section_offset = 0x1414;
          }
          else if (!strcmp(model, "X-T1"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x14b0))
              wb_section_offset = 0x14b0;
          }
          else if (!strcmp(model, "X30"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1444))
              wb_section_offset = 0x1444;
          }
          else if (!strcmp(model, "X100T"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1444))
              wb_section_offset = 0x1444;
          }
          else if (!strcmp(model, "X-T10"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1824))
              wb_section_offset = 0x1824;
          }
          else if (!strcmp(model, "X70"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x17b4))
              wb_section_offset = 0x17b4;
          }
          else if (!strcmp(model, "X-Pro2"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x135c))
              wb_section_offset = 0x135c;
          }
          else if (!strcmp(model, "X-T2"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x13dc))
              wb_section_offset = 0x13dc;
          }
          else if (!strcmp(model, "X-T20"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x13dc))
              wb_section_offset = 0x13dc;
          }
          else if (!strcmp(model, "X100F"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1370))
              wb_section_offset = 0x1370;
          }
          else if (!strcmp(model, "GFX 50S"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1424))
              wb_section_offset = 0x1424;
          }
          else if (!strcmp(model, "GFX 50R"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1424))
              wb_section_offset = 0x1424;
          }
          else if (!strcmp(model, "X-T3"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x2014))
              wb_section_offset = 0x2014;
          }
          else if (!strcmp(model, "X-T30"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x20b8))
              wb_section_offset = 0x2014;
          }
          else if (!strcmp(model, "X-E3"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x141c))
              wb_section_offset = 0x141c;
          }
          else if (!strcmp(model, "X-E2S"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1840))
              wb_section_offset = 0x1840;
          }
          else if (!strcmp(model, "GFX 100"))
          {
            if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x20e4))
              wb_section_offset = 0x20e4;

            /* no RAF Data version for the models below */
          }
          else if (!strcmp(model, "FinePix X100"))
          { // X100 0 0x19f0 0x19e8
            if (!strcmp(imfRAFVersion, "0069"))
              wb_section_offset = 0x19e8;
            else if (!strcmp(imfRAFVersion, "0100"))
              wb_section_offset = 0x19f0;
            else if (!strcmp(imfRAFVersion, "0110"))
              wb_section_offset = 0x19f0;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x19e8))
              wb_section_offset = 0x19e8;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x19f0))
              wb_section_offset = 0x19f0;
          }
          else if (!strcmp(model, "X-E1"))
          { // X-E1 0 0x13ac
            if (!strcmp(imfRAFVersion, "0101"))
              wb_section_offset = 0x13ac;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x13ac))
              wb_section_offset = 0x13ac;
          }
          else if (!strcmp(model, "X-Pro1"))
          { // X-Pro1 0 0x13a4
            if (!strcmp(imfRAFVersion, "0100"))
              wb_section_offset = 0x13a4;
            else if (!strcmp(imfRAFVersion, "0101"))
              wb_section_offset = 0x13a4;
            else if (!strcmp(imfRAFVersion, "0204"))
              wb_section_offset = 0x13a4;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x13a4))
              wb_section_offset = 0x13a4;
          }
          else if (!strcmp(model, "XF1"))
          { // XF1 0 0x138c
            if (!strcmp(imfRAFVersion, "0100"))
              wb_section_offset = 0x138c;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x138c))
              wb_section_offset = 0x138c;
          }
          else if (!strcmp(model, "X-S1"))
          { // X-S1 0 0x1284
            if (!strcmp(imfRAFVersion, "0100"))
              wb_section_offset = 0x1284;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1284))
              wb_section_offset = 0x1284;
          }
          else if (!strcmp(model, "X10"))
          { // X10 0 0x1280 0x12d4
            if (!strcmp(imfRAFVersion, "0100"))
              wb_section_offset = 0x1280;
            else if (!strcmp(imfRAFVersion, "0102"))
              wb_section_offset = 0x1280;
            else if (!strcmp(imfRAFVersion, "0103"))
              wb_section_offset = 0x12d4;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x1280))
              wb_section_offset = 0x1280;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x12d4))
              wb_section_offset = 0x12d4;
          }
          else if (!strcmp(model, "XF1"))
          { // XF1 0 0x138c
            if (!strcmp(imfRAFVersion, "0100"))
              wb_section_offset = 0x138c;
            else if (isWB(PrivateMknBuf + posPrivateMknBuf + 0x138c))
              wb_section_offset = 0x138c;
          }
          if (wb_section_offset &&
              isWB(PrivateMknBuf + posPrivateMknBuf + wb_section_offset))
          {

            if (!imfRAFDataVersion)
            {
              posWB = posPrivateMknBuf + wb_section_offset - 6;
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][1] =
                  imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][3] =
                      sget2(PrivateMknBuf + posWB);
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][0] =
                  sget2(PrivateMknBuf + posWB + 2);
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][2] =
                  sget2(PrivateMknBuf + posWB + 4);
            }

            posWB = posPrivateMknBuf + wb_section_offset;
            for (int wb_ind = 0; wb_ind < nFuji_wb_list1; posWB += 6, wb_ind++)
            {
              imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][1] =
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][3] =
                      sget2(PrivateMknBuf + posWB);
              imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][0] =
                  sget2(PrivateMknBuf + posWB + 2);
              imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][2] =
                  sget2(PrivateMknBuf + posWB + 4);
            }

            int found = 0;
            posWB += 0xC0;
            ushort Gval = sget2(PrivateMknBuf + posWB);
            for (int posEndCCTsection = posWB; posEndCCTsection < (posWB + 30);
                 posEndCCTsection += 6)
            {
              if (sget2(PrivateMknBuf + posEndCCTsection) != Gval)
              {
                wb_section_offset = posEndCCTsection - 186;
                found = 1;
                break;
              }
            }

            if (found)
            {
              for (int iCCT = 0; iCCT < 31; iCCT++)
              {
                imgdata.color.WBCT_Coeffs[iCCT][0] = FujiCCT_K[iCCT];
                imgdata.color.WBCT_Coeffs[iCCT][1] =
                    sget2(PrivateMknBuf + wb_section_offset + iCCT * 6 + 2);
                imgdata.color.WBCT_Coeffs[iCCT][2] =
                    imgdata.color.WBCT_Coeffs[iCCT][4] =
                        sget2(PrivateMknBuf + wb_section_offset + iCCT * 6);
                imgdata.color.WBCT_Coeffs[iCCT][3] =
                    sget2(PrivateMknBuf + wb_section_offset + iCCT * 6 + 4);
              }
            }
          }
        }
        else
        { // process 4K raf data
          int wb[4];
          int nWB, tWB, pWB;
          int iCCT = 0;
          is_4K_RAFdata = 1; /* X-A3, X-A5, X-A7, X-A10, X-A20, X-T100, XF10 */
          posWB = posPrivateMknBuf + 0x200;
          for (int wb_ind = 0; wb_ind < 42; wb_ind++)
          {
            nWB = sget4(PrivateMknBuf + posWB);
            posWB += 4;
            tWB = sget4(PrivateMknBuf + posWB);
            posWB += 4;
            wb[0] = sget4(PrivateMknBuf + posWB) << 1;
            posWB += 4;
            wb[1] = sget4(PrivateMknBuf + posWB);
            posWB += 4;
            wb[3] = sget4(PrivateMknBuf + posWB);
            posWB += 4;
            wb[2] = sget4(PrivateMknBuf + posWB) << 1;
            posWB += 4;

            if (tWB && (iCCT < 255))
            {
              imgdata.color.WBCT_Coeffs[iCCT][0] = tWB;
              FORC4 imgdata.color.WBCT_Coeffs[iCCT][c + 1] = wb[c];
              iCCT++;
            }
            if (nWB != 0x46)
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
      }
      posPrivateMknBuf += PrivateTagBytes;
    }
    free(PrivateMknBuf);
  }
#undef imfRAFVersion
#undef imfRAFDataVersion
#undef icWBC
#undef CHECKSPACE
}