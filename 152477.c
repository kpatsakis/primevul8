void CLASS parse_makernote_0xc634(int base, int uptag, unsigned dng_writer)
{
  unsigned ver97 = 0, offset = 0, entries, tag, type, len, save, c;
  unsigned i;

  uchar NikonKey, ci, cj, ck;
  unsigned serial = 0;
  unsigned NikonLensDataVersion = 0;
  unsigned lenNikonLensData = 0;

  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;

  uchar *table_buf;
  uchar *table_buf_0x9050;
  ushort table_buf_0x9050_present = 0;
  uchar *table_buf_0x940c;
  ushort table_buf_0x940c_present = 0;

  short morder, sorder = order;
  char buf[10];

  fread(buf, 1, 10, ifp);
  if (!strcmp(buf, "Nikon")) {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42) goto quit;
    offset = get4();
    fseek(ifp, offset - 8, SEEK_CUR);
  }
  else if (!strcmp(buf, "OLYMPUS") ||
           !strcmp(buf, "PENTAX ") ||
           (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG))) {
    base = ftell(ifp) - 10;
    fseek(ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O') get2();
  }
  else if (!strncmp(buf, "SONY", 4) ||
           !strcmp(buf, "Panasonic")) {
    goto nf;
  }
  else if (!strncmp(buf, "FUJIFILM", 8)) {
    base = ftell(ifp) - 10;
  nf: order = 0x4949;
    fseek(ifp, 2, SEEK_CUR);
  }
  else if (!strcmp(buf, "OLYMP") ||
           !strcmp(buf, "LEICA") ||
           !strcmp(buf, "Ricoh") ||
           !strcmp(buf, "EPSON"))
    fseek(ifp, -2, SEEK_CUR);
  else if (!strcmp(buf, "AOC") ||
           !strcmp(buf, "QVC"))
    fseek(ifp, -4, SEEK_CUR);
  else {
    fseek(ifp, -10, SEEK_CUR);
    if ((!strncmp(make, "SAMSUNG", 7) &&
				(dng_writer == AdobeDNG)))
      base = ftell(ifp);
  }

  entries = get2();
  if (entries > 1000) return;
  morder = order;

  while (entries--) {
    order = morder;
    tiff_get(base, &tag, &type, &len, &save);
    tag |= uptag << 16;
    if(len > 100*1024*1024) goto next; // 100Mb tag? No!

    if (!strncmp(make, "Canon",5))
      {
        if (tag == 0x0001)				// camera settings
          {
            fseek(ifp, 44, SEEK_CUR);
            imgdata.lens.makernotes.LensID = get2();
            imgdata.lens.makernotes.MaxFocal = get2();
            imgdata.lens.makernotes.MinFocal = get2();
            imgdata.lens.makernotes.CanonFocalUnits = get2();
            if (imgdata.lens.makernotes.CanonFocalUnits != 1)
              {
                imgdata.lens.makernotes.MaxFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
                imgdata.lens.makernotes.MinFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
              }
            imgdata.lens.makernotes.MaxAp = _CanonConvertAperture(get2());
            imgdata.lens.makernotes.MinAp = _CanonConvertAperture(get2());
          }

        else if (tag == 0x0002)			// focal length
          {
            imgdata.lens.makernotes.FocalType = get2();
            imgdata.lens.makernotes.CurFocal = get2();
            if ((imgdata.lens.makernotes.CanonFocalUnits != 1) &&
                imgdata.lens.makernotes.CanonFocalUnits)
              {
                imgdata.lens.makernotes.CurFocal /= (float)imgdata.lens.makernotes.CanonFocalUnits;
              }
          }

        else if (tag == 0x0004)			// shot info
          {
            short tempAp;
            fseek(ifp, 8, SEEK_CUR);
            if ((tempAp = get2()) != 0x7fff)
              imgdata.lens.makernotes.CurAp = _CanonConvertAperture(tempAp);
            if (imgdata.lens.makernotes.CurAp < 0.7f)
            {
              fseek(ifp, 32, SEEK_CUR);
              imgdata.lens.makernotes.CurAp = _CanonConvertAperture(get2());
            }
            if (!aperture) aperture = imgdata.lens.makernotes.CurAp;
          }

        else if (tag == 0x000d)			// camera info
          {
            CanonCameraInfo = (uchar*)malloc(len);
            fread(CanonCameraInfo, len, 1, ifp);
            lenCanonCameraInfo = len;
          }

        else if (tag == 0x10)	// Canon ModelID
          {
            unique_id = get4();
            if (unique_id == 0x03740000) unique_id = 0x80000374;
            setCanonBodyFeatures(unique_id);
            if (lenCanonCameraInfo)
              {
                processCanonCameraInfo(unique_id, CanonCameraInfo,lenCanonCameraInfo);
                free(CanonCameraInfo);
                CanonCameraInfo = 0;
                lenCanonCameraInfo = 0;
              }
          }

        else if (tag == 0x0095 &&		// lens model tag
                 !imgdata.lens.makernotes.Lens[0])
          {
            fread(imgdata.lens.makernotes.Lens, 2, 1, ifp);
            imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
            if (imgdata.lens.makernotes.Lens[0] < 65)					// non-Canon lens
              fread(imgdata.lens.makernotes.Lens + 2, 62, 1, ifp);
            else
              {
                char efs[2];
                imgdata.lens.makernotes.LensFeatures_pre[0] = imgdata.lens.makernotes.Lens[0];
                imgdata.lens.makernotes.LensFeatures_pre[1] = imgdata.lens.makernotes.Lens[1];
                fread(efs, 2, 1, ifp);
                if (efs[0] == 45 && (efs[1] == 83 || efs[1] == 69 || efs[1] == 77))
                  {	// "EF-S, TS-E, MP-E, EF-M" lenses
                    imgdata.lens.makernotes.Lens[2] = imgdata.lens.makernotes.LensFeatures_pre[2] = efs[0];
                    imgdata.lens.makernotes.Lens[3] = imgdata.lens.makernotes.LensFeatures_pre[3] = efs[1];
                    imgdata.lens.makernotes.Lens[4] = 32;
                    if (efs[1] == 83)
                      {
                        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF_S;
                        imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
                      }
                    else if (efs[1] == 77)
                      {
                        imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF_M;
                      }
                  }
                else
                  {																// "EF" lenses
                    imgdata.lens.makernotes.Lens[2] = 32;
                    imgdata.lens.makernotes.Lens[3] = efs[0];
                    imgdata.lens.makernotes.Lens[4] = efs[1];
                  }
                fread(imgdata.lens.makernotes.Lens + 5, 58, 1, ifp);
              }
          }
      }

    else if (!strncmp(make, "FUJI", 4))
      switch (tag) {
      case 0x1404: imgdata.lens.makernotes.MinFocal = getreal(type); break;
      case 0x1405: imgdata.lens.makernotes.MaxFocal = getreal(type); break;
      case 0x1406: imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type); break;
      case 0x1407: imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type); break;
      }

    else if (!strncasecmp(make, "LEICA", 5))
      {
        if ((tag == 0x0303) && (type != 4))
          {
            fread(imgdata.lens.makernotes.Lens, MIN(len,127), 1, ifp);
          }

        if ((tag == 0x3405) ||
            (tag == 0x0310) ||
            (tag == 0x34003405))
          {
            imgdata.lens.makernotes.LensID = get4();
            imgdata.lens.makernotes.LensID =
              ((imgdata.lens.makernotes.LensID>>2)<<8) |
              (imgdata.lens.makernotes.LensID & 0x3);
            if (imgdata.lens.makernotes.LensID != -1)
              {
                if ((model[0] == 'M') ||
                    !strncasecmp (model, "LEICA M", 7))
                  {
                    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_M;
                    if (imgdata.lens.makernotes.LensID)
                      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Leica_M;
                  }
                else if ((model[0] == 'S') ||
                         !strncasecmp (model, "LEICA S", 7))
                  {
                    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_S;
                    if (imgdata.lens.makernotes.Lens[0])
                      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Leica_S;
                  }
              }
          }

        else if (
                 ((tag == 0x0313) || (tag == 0x34003406)) &&
                 (fabs(imgdata.lens.makernotes.CurAp) < 0.17f) &&
                 ((type == 10) || (type == 5))
                )
          {
            imgdata.lens.makernotes.CurAp = getreal(type);
            if (imgdata.lens.makernotes.CurAp > 126.3)
              imgdata.lens.makernotes.CurAp = 0.0f;
          }

        else if (tag == 0x3400)
          {
            parse_makernote (base, 0x3400);
          }
      }

    else if (!strncmp(make, "NIKON", 5))
      {
        if (tag == 0x1d)							// serial number
          while ((c = fgetc(ifp)) && c != EOF)
            serial = serial * 10 + (isdigit(c) ? c - '0' : c % 10);
        else if (tag == 0x000a)
          {
            imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
            imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
          }
        else if (tag == 0x0082)				// lens attachment
          {
            fread(imgdata.lens.makernotes.Attachment, MIN(len,127), 1, ifp);
          }
        else if (tag == 0x0083)				// lens type
          {
            imgdata.lens.nikon.NikonLensType = fgetc(ifp);
          }
        else if (tag == 0x0084)				// lens
          {
            imgdata.lens.makernotes.MinFocal = getreal(type);
            imgdata.lens.makernotes.MaxFocal = getreal(type);
            imgdata.lens.makernotes.MaxAp4MinFocal = getreal(type);
            imgdata.lens.makernotes.MaxAp4MaxFocal = getreal(type);
          }
        else if (tag == 0x008b)				// lens f-stops
          {
            uchar a, b, c;
            a = fgetc(ifp);
            b = fgetc(ifp);
            c = fgetc(ifp);
            if (c)
              {
                imgdata.lens.nikon.NikonLensFStops = a*b*(12/c);
                imgdata.lens.makernotes.LensFStops =
                  (float)imgdata.lens.nikon.NikonLensFStops /12.0f;
              }
          }
        else if (tag == 0x0093)
          {
            i = get2();
            if ((i == 7) || (i == 9))
            {
              imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
              imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
            }
          }
        else if (tag == 0x0097)
          {
            for (i=0; i < 4; i++)
	            ver97 = ver97 * 10 + fgetc(ifp)-'0';
	          if (ver97 == 601)  // Coolpix A
	          {
	            imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
                    imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
	          }
	        }
        else if (tag == 0x0098)				// contains lens data
          {
            for (i = 0; i < 4; i++)
              {
                NikonLensDataVersion = NikonLensDataVersion * 10 + fgetc(ifp) - '0';
              }
            switch (NikonLensDataVersion)
              {
              case 100: lenNikonLensData = 9; break;
              case 101:
              case 201:	// encrypted, starting from v.201
              case 202:
              case 203: lenNikonLensData = 15; break;
              case 204: lenNikonLensData = 16; break;
              case 400: lenNikonLensData = 459; break;
              case 401: lenNikonLensData = 590; break;
              case 402: lenNikonLensData = 509; break;
              case 403: lenNikonLensData = 879; break;
              }
            if(lenNikonLensData)
              {
                table_buf = (uchar*)malloc(lenNikonLensData);
                fread(table_buf, lenNikonLensData, 1, ifp);
                if ((NikonLensDataVersion < 201) && lenNikonLensData)
                  {
                    processNikonLensData(table_buf, lenNikonLensData);
                    free(table_buf);
                    lenNikonLensData = 0;
                  }
              }
          }

        else if (tag == 0xa7)					// shutter count
          {
            NikonKey = fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp);
            if ((NikonLensDataVersion > 200) && lenNikonLensData)
              {
                ci = xlat[0][serial & 0xff];
                cj = xlat[1][NikonKey];
                ck = 0x60;
                for (i = 0; i < lenNikonLensData; i++)
                  table_buf[i] ^= (cj += ci * ck++);
                processNikonLensData(table_buf, lenNikonLensData);
                free(table_buf);
                lenNikonLensData = 0;
              }
          }

        else if (tag == 37 && (!iso_speed || iso_speed == 65535))
          {
            unsigned char cc;
            fread(&cc, 1, 1, ifp);
            iso_speed = (int)(100.0 * powf64(2.0, (double)(cc) / 12.0 - 5.0));
            break;
          }
      }

    else if (!strncmp(make, "OLYMPUS", 7))
      {
        if (tag == 0x2010)
          {
            fseek(ifp, save - 4, SEEK_SET);
            fseek(ifp, base + get4(), SEEK_SET);
            parse_makernote_0xc634(base, 0x2010, dng_writer);
          }

        switch (tag) {
        case 0x0207:
        case 0x20100100:
          {
            uchar sOlyID[8];
            unsigned long long OlyID;
            fread (sOlyID, MIN(len,7), 1, ifp);
	    sOlyID[7] = 0;
            OlyID = sOlyID[0];
            i = 1;
            while (i < 7 && sOlyID[i])
              {
                OlyID = OlyID << 8 | sOlyID[i];
                i++;
              }
            setOlympusBodyFeatures(OlyID);
          }
          break;
        case 0x1002:
          imgdata.lens.makernotes.CurAp = powf64(2.0f, getreal(type)/2);
          break;
        case 0x20100201:
          imgdata.lens.makernotes.LensID =
            (unsigned long long)fgetc(ifp)<<16 |
            (unsigned long long)(fgetc(ifp), fgetc(ifp))<<8 |
            (unsigned long long)fgetc(ifp);
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FT;
          imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_FT;
          if (((imgdata.lens.makernotes.LensID < 0x20000) ||
               (imgdata.lens.makernotes.LensID > 0x4ffff)) &&
              (imgdata.lens.makernotes.LensID & 0x10))
            {
              imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_mFT;
            }
          break;
        case 0x20100203:
          fread(imgdata.lens.makernotes.Lens, MIN(len,127), 1, ifp);
          break;
        case 0x20100205:
          imgdata.lens.makernotes.MaxAp4MinFocal = powf64(sqrt(2.0f), get2() / 256.0f);
          break;
        case 0x20100206:
          imgdata.lens.makernotes.MaxAp4MaxFocal = powf64(sqrt(2.0f), get2() / 256.0f);
          break;
        case 0x20100207:
          imgdata.lens.makernotes.MinFocal = (float)get2();
          break;
        case 0x20100208:
          imgdata.lens.makernotes.MaxFocal = (float)get2();
          if (imgdata.lens.makernotes.MaxFocal > 1000.0f)
            imgdata.lens.makernotes.MaxFocal = imgdata.lens.makernotes.MinFocal;
          break;
        case 0x2010020a:
          imgdata.lens.makernotes.MaxAp4CurFocal = powf64(sqrt(2.0f), get2() / 256.0f);
          break;
        case 0x20100301:
          imgdata.lens.makernotes.TeleconverterID = fgetc(ifp) << 8;
          fgetc(ifp);
          imgdata.lens.makernotes.TeleconverterID =
            imgdata.lens.makernotes.TeleconverterID | fgetc(ifp);
          break;
        case 0x20100303:
          fread(imgdata.lens.makernotes.Teleconverter, MIN(len,127), 1, ifp);
          break;
        case 0x20100403:
          fread(imgdata.lens.makernotes.Attachment, MIN(len,127), 1, ifp);
          break;
        }
      }

    else if (!strncmp(make, "PENTAX", 6) ||
             !strncmp(model, "PENTAX", 6) ||
             (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG)))
      {
        if (tag == 0x0005)
          {
            unique_id = get4();
            setPentaxBodyFeatures(unique_id);
          }
        else if (tag == 0x0013)
          {
            imgdata.lens.makernotes.CurAp = (float)get2()/10.0f;
          }
        else if (tag == 0x001d)
          {
            imgdata.lens.makernotes.CurFocal = (float)get4()/100.0f;
          }
        else if (tag == 0x003f)
          {
            imgdata.lens.makernotes.LensID = fgetc(ifp) << 8 | fgetc(ifp);
          }
        else if (tag == 0x0207)
          {
            PentaxLensInfo(imgdata.lens.makernotes.CamID, len);
          }
        else if (tag == 0x0239)		// Q-series lens info (LensInfoQ)
          {
            char LensInfo [20];
            fseek (ifp, 12, SEEK_CUR);
            fread(imgdata.lens.makernotes.Lens, 30, 1, ifp);
            strcat(imgdata.lens.makernotes.Lens, " ");
            fread(LensInfo, 20, 1, ifp);
            strcat(imgdata.lens.makernotes.Lens, LensInfo);
          }
      }

    else if (!strncmp(make, "SAMSUNG", 7) &&
             (dng_writer == AdobeDNG))
      {
        if (tag == 0x0002)
          {
            if(get4() == 0x2000)
              {
                imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Samsung_NX;
              }
            else if (!strncmp(model, "NX mini", 7))
              {
                imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Samsung_NX_M;
              }
            else
              {
                imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
                imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
              }
          }
        else if (tag == 0x0003)
          {
            imgdata.lens.makernotes.CamID = unique_id = get4();
          }
        else if (tag == 0xa003)
          {
            imgdata.lens.makernotes.LensID = get2();
            if (imgdata.lens.makernotes.LensID)
              imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Samsung_NX;
          }
        else if (tag == 0xa019)
          {
            imgdata.lens.makernotes.CurAp = getreal(type);
          }
        else if (tag == 0xa01a)
          {
            imgdata.lens.makernotes.FocalLengthIn35mmFormat = get4() / 10.0f;
            if (imgdata.lens.makernotes.FocalLengthIn35mmFormat < 10.0f)
              imgdata.lens.makernotes.FocalLengthIn35mmFormat *= 10.0f;
          }
      }

    else if (!strncasecmp(make, "SONY", 4) ||
             !strncasecmp(make, "Konica", 6) ||
             !strncasecmp(make, "Minolta", 7) ||
             (!strncasecmp(make, "Hasselblad", 10) &&
              (!strncasecmp(model, "Stellar", 7) ||
               !strncasecmp(model, "Lunar", 5) ||
               !strncasecmp(model, "Lusso", 5) ||
               !strncasecmp(model, "HV",2))))
      {
        ushort lid;

        if (tag == 0xb001)			// Sony ModelID
          {
            unique_id = get2();
            setSonyBodyFeatures(unique_id);
            if (table_buf_0x9050_present)
              {
                process_Sony_0x9050(table_buf_0x9050, unique_id);
                free (table_buf_0x9050);
                table_buf_0x9050_present = 0;
              }
            if (table_buf_0x940c_present)
              {
                if (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E)
                  {
                    process_Sony_0x940c(table_buf_0x940c);
                  }
                free (table_buf_0x940c);
                table_buf_0x940c_present = 0;
              }
          }
        else if ((tag == 0x0010) &&					// CameraInfo
                 strncasecmp(model, "DSLR-A100", 9) &&
                 strncasecmp(model, "NEX-5C", 6) &&
                 !strncasecmp(make, "SONY", 4) &&
                 ((len == 368) ||			// a700
                  (len == 5478) ||		// a850, a900
                  (len == 5506) ||		// a200, a300, a350
                  (len == 6118) ||		// a230, a290, a330, a380, a390

                  // a450, a500, a550, a560, a580
                  // a33, a35, a55
                  // NEX3, NEX5, NEX5C, NEXC3, VG10E
                  (len == 15360))
                 )
          {
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            if (memcmp(table_buf, "\xff\xff\xff\xff\xff\xff\xff\xff", 8) &&
                memcmp(table_buf, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))
              {
                switch (len) {
                case 368:
                case 5478:
                  // a700, a850, a900: CameraInfo
                  if (saneSonyCameraInfo(table_buf[0], table_buf[3], table_buf[2], table_buf[5], table_buf[4], table_buf[7]))
                    {
                      if (table_buf[0] | table_buf[3])
                        imgdata.lens.makernotes.MinFocal =
                          bcd2dec(table_buf[0]) * 100 + bcd2dec(table_buf[3]);
                      if (table_buf[2] | table_buf[5])
                        imgdata.lens.makernotes.MaxFocal =
                          bcd2dec(table_buf[2]) * 100 + bcd2dec(table_buf[5]);
                      if (table_buf[4])
                        imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[4]) / 10.0f;
                      if (table_buf[4])
                        imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[7]) / 10.0f;
                      parseSonyLensFeatures(table_buf[1], table_buf[6]);
                    }
                  break;
                default:
                  // CameraInfo2 & 3
                  if (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3], table_buf[4], table_buf[5], table_buf[6]))
                    {
                      if (table_buf[1] | table_buf[2])
                        imgdata.lens.makernotes.MinFocal =
                          bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
                      if (table_buf[3] | table_buf[4])
                        imgdata.lens.makernotes.MaxFocal =
                          bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
                      if (table_buf[5])
                        imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
                      if (table_buf[6])
                        imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
                      parseSonyLensFeatures(table_buf[0], table_buf[7]);
                    }
                }
              }
            free(table_buf);
          }

        else if (tag == 0x0105)					// Teleconverter
          {
            imgdata.lens.makernotes.TeleconverterID = get2();
          }

        else if (tag == 0x0114)					// CameraSettings
          {
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            switch (len) {
            case 280:
            case 364:
            case 332:
              // CameraSettings and CameraSettings2 are big endian
              if (table_buf[2] | table_buf[3])
                {
                  lid = (((ushort)table_buf[2])<<8) |
                    ((ushort)table_buf[3]);
                  imgdata.lens.makernotes.CurAp =
                    powf64(2.0f, ((float)lid/8.0f-1.0f)/2.0f);
                }
              break;
            case 1536:
            case 2048:
              // CameraSettings3 are little endian
              parseSonyLensType2(table_buf[1016], table_buf[1015]);
              if (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Canon_EF)
                {
                  switch (table_buf[153]) {
                  case 16: imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Minolta_A; break;
                  case 17: imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E; break;
                  }
                }
              break;
            }
            free(table_buf);
          }

        else if (tag == 0x9050)		// little endian
          {
            table_buf_0x9050 = (uchar*)malloc(len);
            table_buf_0x9050_present = 1;
            fread(table_buf_0x9050, len, 1, ifp);

            if (imgdata.lens.makernotes.CamID)
              {
                process_Sony_0x9050(table_buf_0x9050, imgdata.lens.makernotes.CamID);
                free (table_buf_0x9050);
                table_buf_0x9050_present = 0;
              }
          }

        else if (tag == 0x940c)
          {
            table_buf_0x940c = (uchar*)malloc(len);
            table_buf_0x940c_present = 1;
            fread(table_buf_0x940c, len, 1, ifp);
            if ((imgdata.lens.makernotes.CamID) &&
                (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E))
              {
                process_Sony_0x940c(table_buf_0x940c);
                free(table_buf_0x940c);
                table_buf_0x940c_present = 0;
              }
          }

        else if (((tag == 0xb027) || (tag == 0x010c)) && (imgdata.lens.makernotes.LensID == -1))
          {
            imgdata.lens.makernotes.LensID = get4();
            if ((imgdata.lens.makernotes.LensID > 61184) &&
                (imgdata.lens.makernotes.LensID < 65535))
              {
                imgdata.lens.makernotes.LensID -= 61184;
                imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
              }
            if (tag == 0x010c) imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Minolta_A;
          }

        else if (tag == 0xb02a)					// Sony LensSpec
          {
            table_buf = (uchar*)malloc(len);
            fread(table_buf, len, 1, ifp);
            if (saneSonyCameraInfo(table_buf[1], table_buf[2], table_buf[3], table_buf[4], table_buf[5], table_buf[6]))
              {
                if (table_buf[1] | table_buf[2])
                  imgdata.lens.makernotes.MinFocal =
                    bcd2dec(table_buf[1]) * 100 + bcd2dec(table_buf[2]);
                if (table_buf[3] | table_buf[4])
                  imgdata.lens.makernotes.MaxFocal =
                    bcd2dec(table_buf[3]) * 100 + bcd2dec(table_buf[4]);
                if (table_buf[5])
                  imgdata.lens.makernotes.MaxAp4MinFocal = bcd2dec(table_buf[5]) / 10.0f;
                if (table_buf[6])
                  imgdata.lens.makernotes.MaxAp4MaxFocal = bcd2dec(table_buf[6]) / 10.0f;
                parseSonyLensFeatures(table_buf[0], table_buf[7]);
              }
            free(table_buf);
          }
      }
  next:
    fseek (ifp, save, SEEK_SET);
  }
 quit:
  order = sorder;
}