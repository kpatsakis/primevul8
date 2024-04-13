void CLASS parse_makernote (int base, int uptag)
{
  unsigned offset=0, entries, tag, type, len, save, c;
  unsigned ver97=0, serial=0, i, wbi=0, wb[4]={0,0,0,0};
  uchar buf97[324], ci, cj, ck;
  short morder, sorder=order;
  char buf[10];
  unsigned SamsungKey[11];
  static const double rgb_adobe[3][3] =		// inv(sRGB2XYZ_D65) * AdobeRGB2XYZ_D65
    {{ 1.398283396477404,     -0.398283116703571, 4.427165001263944E-08},
     {-1.233904514232401E-07,  0.999999995196570, 3.126724276714121e-08},
     { 4.561487232726535E-08, -0.042938290466635, 1.042938250416105    }};

  float adobe_cam [3][3];
  uchar NikonKey;

#ifdef LIBRAW_LIBRARY_BUILD
  unsigned NikonLensDataVersion = 0;
  unsigned lenNikonLensData = 0;

  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;

  uchar *table_buf;
  uchar *table_buf_0x9050;
  ushort table_buf_0x9050_present = 0;
  uchar *table_buf_0x940c;
  ushort table_buf_0x940c_present = 0;
#endif
/*
   The MakerNote might have its own TIFF header (possibly with
   its own byte-order!), or it might just be a table.
 */
  if (!strncmp(make,"Nokia",5)) return;
  fread (buf, 1, 10, ifp);
  if (!strncmp (buf,"KDK" ,3) ||	/* these aren't TIFF tables */
      !strncmp (buf,"VER" ,3) ||
      !strncmp (buf,"IIII",4) ||
      !strncmp (buf,"MMMM",4)) return;
  if (!strncmp (buf,"KC"  ,2) ||	/* Konica KD-400Z, KD-510Z */
      !strncmp (buf,"MLY" ,3)) {	/* Minolta DiMAGE G series */
    order = 0x4d4d;
    while ((i=ftell(ifp)) < data_offset && i < 16384) {
      wb[0] = wb[2];  wb[2] = wb[1];  wb[1] = wb[3];
      wb[3] = get2();
      if (wb[1] == 256 && wb[3] == 256 &&
	  wb[0] > 256 && wb[0] < 640 && wb[2] > 256 && wb[2] < 640)
	FORC4 cam_mul[c] = wb[c];
    }
    goto quit;
  }
  if (!strcmp (buf,"Nikon")) {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42) goto quit;
    offset = get4();
    fseek (ifp, offset-8, SEEK_CUR);
  } else if (!strcmp (buf,"OLYMPUS") ||
             !strcmp (buf,"PENTAX ")) {
    base = ftell(ifp)-10;
    fseek (ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O') get2();
  } else if (!strncmp (buf,"SONY",4) ||
             !strcmp  (buf,"Panasonic")) {
    goto nf;
  } else if (!strncmp (buf,"FUJIFILM",8)) {
    base = ftell(ifp)-10;
	nf: order = 0x4949;
    fseek (ifp,  2, SEEK_CUR);
  } else if (!strcmp (buf,"OLYMP") ||
             !strcmp (buf,"LEICA") ||
             !strcmp (buf,"Ricoh") ||
             !strcmp (buf,"EPSON"))
    fseek (ifp, -2, SEEK_CUR);
  else if (!strcmp (buf,"AOC") ||
           !strcmp (buf,"QVC"))
    fseek (ifp, -4, SEEK_CUR);
  else {
    fseek (ifp, -10, SEEK_CUR);
    if (!strncmp(make,"SAMSUNG",7))
      base = ftell(ifp);
  }

  // adjust pos & base for Leica M8/M9/M Mono tags and dir in tag 0x3400
  if (!strncasecmp(make, "LEICA", 5))
    {
      if (!strncmp(model, "M8", 2) ||
          !strncasecmp(model, "Leica M8", 8) ||
          !strncasecmp(model, "LEICA X", 7))
        {
          base = ftell(ifp)-8;
        }
      else if (!strncasecmp(model, "LEICA M (Typ 240)", 17))
        {
          base = 0;
        }
      else if (!strncmp(model, "M9", 2) ||
               !strncasecmp(model, "Leica M9", 8) ||
               !strncasecmp(model, "M Monochrom", 11) ||
               !strncasecmp(model, "Leica M Monochrom", 11))
        {
          if (!uptag)
            {
              base = ftell(ifp) - 10;
              fseek (ifp, 8, SEEK_CUR);
            }
          else if (uptag == 0x3400)
            {
              fseek (ifp, 10, SEEK_CUR);
              base += 10;
            }
        }
      else if (!strncasecmp(model, "LEICA T", 7))
      	{
      	  base = ftell(ifp)-8;
#ifdef LIBRAW_LIBRARY_BUILD
      	  imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_T;
#endif
      	}
    }

  entries = get2();
  if (entries > 1000) return;
  morder = order;
  while (entries--) {
    order = morder;
    tiff_get (base, &tag, &type, &len, &save);
    tag |= uptag << 16;
    if(len > 100*1024*1024) continue; // 100Mb tag? No!

#ifdef LIBRAW_LIBRARY_BUILD
    INT64 _pos = ftell(ifp);
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

    else if (!strncmp(make, "NIKON",5))
      {
        if (tag == 0x000a)
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
            if(lenNikonLensData>0)
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
      }

    else if (!strncmp(make, "OLYMPUS", 7))
      {
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
          {
            unsigned long long oly_lensid [3];
            oly_lensid[0] = fgetc(ifp);
            fgetc(ifp);
            oly_lensid[1] = fgetc(ifp);
            oly_lensid[2] = fgetc(ifp);
            imgdata.lens.makernotes.LensID =
              (oly_lensid[0] << 16) | (oly_lensid[1] << 8) | oly_lensid[2];
          }
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

    else if ((!strncmp(make, "PENTAX", 6) || !strncmp(make, "RICOH", 5)) &&
             !strncmp(model, "GR", 2))
      {
        if ((tag == 0x1001) && (type == 3))
          {
            imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
            imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
            imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
            imgdata.lens.makernotes.LensID = -1;
            imgdata.lens.makernotes.FocalType = 1;
          }
        else if ((tag == 0x1017) && (get2() == 2))
          {
            strcpy(imgdata.lens.makernotes.Attachment, "Wide-Angle Adapter");
          }
        else if (tag == 0x1500)
          {
            imgdata.lens.makernotes.CurFocal = getreal(type);
          }
      }

    else if (!strncmp(make, "RICOH", 5) &&
             strncmp(model, "PENTAX", 6))
      {
        if ((tag == 0x1017) && (get2() == 2))
          {
            strcpy(imgdata.lens.makernotes.Attachment, "Wide-Angle Adapter");
          }

        else if (tag == 0x1500)
          {
            imgdata.lens.makernotes.CurFocal = getreal(type);
          }

        else if ((tag == 0x2001) && !strncmp(model, "GXR", 3))
          {
            short ntags, cur_tag;
            fseek(ifp, 20, SEEK_CUR);
            ntags = get2();
            cur_tag = get2();
            while (cur_tag != 0x002c)
              {
                fseek(ifp, 10, SEEK_CUR);
                cur_tag = get2();
              }
            fseek(ifp, 6, SEEK_CUR);
            fseek(ifp, get4()+34, SEEK_SET);
            imgdata.lens.makernotes.LensID = getc(ifp) - '0';
            switch(imgdata.lens.makernotes.LensID)
              {
            	case 1:
            	case 2:
            	case 3:
            	case 5:
            	case 6:
            		imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
                imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_RicohModule;
                break;
              case 8:
                imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Leica_M;
                imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
                imgdata.lens.makernotes.LensID = -1;
                break;
              default:
              	imgdata.lens.makernotes.LensID = -1;
              }
          }
      }

    else if (!strncmp(make, "PENTAX", 6) ||
             !strncmp(model, "PENTAX", 6) ||
             (!strncmp(make, "SAMSUNG", 7) && dng_version) &&
             strncmp(model, "GR", 2))
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
            fseek (ifp, 2, SEEK_CUR);
            fread(imgdata.lens.makernotes.Lens, 30, 1, ifp);
            strcat(imgdata.lens.makernotes.Lens, " ");
            fread(LensInfo, 20, 1, ifp);
            strcat(imgdata.lens.makernotes.Lens, LensInfo);
          }
      }

    else if (!strncmp(make, "SAMSUNG", 7))
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
            unique_id = imgdata.lens.makernotes.CamID = get4();
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
                switch (len)
                  {
                  case 368:
                  case 5478:
                    // a700, a850, a900: CameraInfo
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
                    break;
                  default:
                    // CameraInfo2 & 3
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
            free(table_buf);
          }
      }

    fseek(ifp,_pos,SEEK_SET);
#endif

    if (tag == 2 && strstr(make,"NIKON") && !iso_speed)
      iso_speed = (get2(),get2());
    if (tag == 37 && strstr(make,"NIKON") && (!iso_speed || iso_speed == 65535))
      {
        unsigned char cc;
        fread(&cc,1,1,ifp);
        iso_speed = int(100.0 * powf64(2.0f,float(cc)/12.0-5.0));
      }
    if (tag == 4 && len > 26 && len < 35) {
      if ((i=(get4(),get2())) != 0x7fff && (!iso_speed || iso_speed == 65535))
	iso_speed = 50 * powf64(2.0, i/32.0 - 4);
#ifdef LIBRAW_LIBRARY_BUILD
      get4();
#else
      if ((i=(get2(),get2())) != 0x7fff && !aperture)
	aperture = powf64(2.0, i/64.0);
#endif
      if ((i=get2()) != 0xffff && !shutter)
	shutter = powf64(2.0, (short) i/-32.0);
      wbi = (get2(),get2());
      shot_order = (get2(),get2());
    }
    if ((tag == 4 || tag == 0x114) && !strncmp(make,"KONICA",6)) {
      fseek (ifp, tag == 4 ? 140:160, SEEK_CUR);
      switch (get2()) {
	case 72:  flip = 0;  break;
	case 76:  flip = 6;  break;
	case 82:  flip = 5;  break;
      }
    }
    if (tag == 7 && type == 2 && len > 20)
      fgets (model2, 64, ifp);
    if (tag == 8 && type == 4)
      shot_order = get4();
    if (tag == 9 && !strncmp(make,"Canon",5))
      fread (artist, 64, 1, ifp);
    if (tag == 0xc && len == 4)
      FORC3 cam_mul[(c << 1 | c >> 1) & 3] = getreal(type);
    if (tag == 0xd && type == 7 && get2() == 0xaaaa) {
      for (c=i=2; (ushort) c != 0xbbbb && i < len; i++)
	c = c << 8 | fgetc(ifp);
      while ((i+=4) < len-5)
	if (get4() == 257 && (i=len) && (c = (get4(),fgetc(ifp))) < 3)
	  flip = "065"[c]-'0';
    }

    if (tag == 0x10 && type == 4)
      {
        unique_id = get4();

#ifdef LIBRAW_LIBRARY_BUILD
        if (unique_id == 0x03740000) unique_id = 0x80000374;
        setCanonBodyFeatures(unique_id);
        if (lenCanonCameraInfo)
          {
            processCanonCameraInfo(unique_id, CanonCameraInfo,lenCanonCameraInfo);
            free(CanonCameraInfo);
            CanonCameraInfo = 0;
            lenCanonCameraInfo = 0;
          }
#endif
      }

#ifdef LIBRAW_LIBRARY_BUILD
    INT64 _pos2 = ftell(ifp);

    if(tag == 0x20400805 && len == 2 && !strncasecmp(make,"Olympus",7))
      {
        imgdata.color.OlympusSensorCalibration[0]=getreal(type);
        imgdata.color.OlympusSensorCalibration[1]=getreal(type);
      }
    if (tag == 0x4001 && len > 500 && !strncasecmp(make,"Canon",5))
      {
        long int save1 = ftell(ifp);
        switch (len)
          {
          case 582:
            imgdata.color.canon_makernotes.CanonColorDataVer = 1;	// 20D / 350D
            break;
          case 653:
            imgdata.color.canon_makernotes.CanonColorDataVer = 2;	// 1Dmk2 / 1DsMK2
            break;
          case 796:
            imgdata.color.canon_makernotes.CanonColorDataVer = 3;	// 1DmkIIN / 5D / 30D / 400D
			imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
			{
			  fseek (ifp, save1+(0x0c4<<1), SEEK_SET); // offset 196 short
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
			}
			break;
            // 1DmkIII / 1DSmkIII / 1DmkIV / 5DmkII
            // 7D / 40D / 50D / 60D / 450D / 500D
            // 550D / 1000D / 1100D
          case 674: case 692: case 702: case 1227: case 1250:
          case 1251: case 1337: case 1338: case 1346:
            imgdata.color.canon_makernotes.CanonColorDataVer = 4;
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x0e7<<1), SEEK_SET); // offset 231 short
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            if ((imgdata.color.canon_makernotes.CanonColorDataSubVer == 4)
                || (imgdata.color.canon_makernotes.CanonColorDataSubVer == 5))
              {
                fseek (ifp, save1+(0x2b9<<1), SEEK_SET);		// offset 697 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            else if ((imgdata.color.canon_makernotes.CanonColorDataSubVer == 6) ||
                     (imgdata.color.canon_makernotes.CanonColorDataSubVer == 7))
              {
                fseek (ifp, save1+(0x2d0<<1), SEEK_SET);		// offset 720 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            else if (imgdata.color.canon_makernotes.CanonColorDataSubVer == 9)
              {
                fseek (ifp, save1+(0x2d4<<1), SEEK_SET);		// offset 724 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            break;

          case 5120:
            imgdata.color.canon_makernotes.CanonColorDataVer = 5;	// PowerSot G10, EOS M3
            {
              fseek (ifp, save1+(0x108<<1), SEEK_SET);			// offset 264 short
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            break;

          case 1273: case 1275:
            imgdata.color.canon_makernotes.CanonColorDataVer = 6;	// 600D / 1200D
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x0fb<<1), SEEK_SET);			// offset 251 short
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            fseek (ifp, save1+(0x1e4<<1), SEEK_SET);			// offset 484 shorts
            imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
            break;

            // 1DX / 5DmkIII / 6D / 100D / 650D / 700D / M / 7DmkII / 750D / 760D
          case 1312: case 1313: case 1316: case 1506:
            imgdata.color.canon_makernotes.CanonColorDataVer = 7;
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x114<<1), SEEK_SET);			// offset 276 shorts
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
            if (imgdata.color.canon_makernotes.CanonColorDataSubVer == 10)
              {
                fseek (ifp, save1+(0x1fd<<1), SEEK_SET);		// offset 509 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              } else if (imgdata.color.canon_makernotes.CanonColorDataSubVer == 11)
              {
                fseek (ifp, save1+(0x2dd<<1), SEEK_SET);		// offset 733 shorts
                imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
              }
            break;

            // 5DS / 5DS R
          case 1560:
            imgdata.color.canon_makernotes.CanonColorDataVer = 8;
            imgdata.color.canon_makernotes.CanonColorDataSubVer = get2();
            {
              fseek (ifp, save1+(0x146<<1), SEEK_SET);			// offset 326 shorts
              int bls=0;
              FORC4 bls+=get2();
              imgdata.color.canon_makernotes.AverageBlackLevel = bls/4;
            }
              fseek (ifp, save1+(0x30f<<1), SEEK_SET);		// offset 783 shorts
              imgdata.color.canon_makernotes.SpecularWhiteLevel = get2();
            break;

          }
        fseek (ifp, save1, SEEK_SET);
      }

    fseek(ifp,_pos2,SEEK_SET);

#endif
    if (tag == 0x11 && is_raw && !strncmp(make,"NIKON",5)) {
      fseek (ifp, get4()+base, SEEK_SET);
      parse_tiff_ifd (base);
    }
    if (tag == 0x14 && type == 7) {
      if (len == 2560) {
	fseek (ifp, 1248, SEEK_CUR);
	goto get2_256;
      }
      fread (buf, 1, 10, ifp);
      if (!strncmp(buf,"NRW ",4)) {
	fseek (ifp, strcmp(buf+4,"0100") ? 46:1546, SEEK_CUR);
	cam_mul[0] = get4() << 2;
	cam_mul[1] = get4() + get4();
	cam_mul[2] = get4() << 2;
      }
    }
    if (tag == 0x15 && type == 2 && is_raw)
      fread (model, 64, 1, ifp);
    if (strstr(make,"PENTAX")) {
      if (tag == 0x1b) tag = 0x1018;
      if (tag == 0x1c) tag = 0x1017;
    }
    if (tag == 0x1d)
      while ((c = fgetc(ifp)) && c != EOF)
	serial = serial*10 + (isdigit(c) ? c - '0' : c % 10);
    if (tag == 0x29 && type == 1) {  // Canon PowerShot G9
      c = wbi < 18 ? "012347800000005896"[wbi]-'0' : 0;
      fseek (ifp, 8 + c*32, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get4();
    }
#ifndef LIBRAW_LIBRARY_BUILD
	// works for some files, but not all
    if (tag == 0x3d && type == 3 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2() >> (14-tiff_ifd[2].bps);
#endif
    if (tag == 0x81 && type == 4) {
      data_offset = get4();
      fseek (ifp, data_offset + 41, SEEK_SET);
      raw_height = get2() * 2;
      raw_width  = get2();
      filters = 0x61616161;
    }
    if ((tag == 0x81  && type == 7) ||
	(tag == 0x100 && type == 7) ||
	(tag == 0x280 && type == 1)) {
      thumb_offset = ftell(ifp);
      thumb_length = len;
    }
    if (tag == 0x88 && type == 4 && (thumb_offset = get4()))
      thumb_offset += base;
    if (tag == 0x89 && type == 4)
      thumb_length = get4();
    if (tag == 0x8c || tag == 0x96)
      meta_offset = ftell(ifp);
    if (tag == 0x97) {
      for (i=0; i < 4; i++)
	ver97 = ver97 * 10 + fgetc(ifp)-'0';
      switch (ver97) {
	case 100:
	  fseek (ifp, 68, SEEK_CUR);
	  FORC4 cam_mul[(c >> 1) | ((c & 1) << 1)] = get2();
	  break;
	case 102:
	  fseek (ifp, 6, SEEK_CUR);
	  FORC4 cam_mul[c ^ (c >> 1)] = get2();
	  break;
	case 103:
	  fseek (ifp, 16, SEEK_CUR);
	  FORC4 cam_mul[c] = get2();
      }
      if (ver97 >= 200) {
	if (ver97 != 205) fseek (ifp, 280, SEEK_CUR);
	fread (buf97, 324, 1, ifp);
      }
    }
    if (tag == 0xa1 && type == 7) {
      order = 0x4949;
      fseek (ifp, 140, SEEK_CUR);
      FORC3 cam_mul[c] = get4();
    }
    if (tag == 0xa4 && type == 3) {
      fseek (ifp, wbi*48, SEEK_CUR);
      FORC3 cam_mul[c] = get2();
    }

    if (tag == 0xa7) {	// shutter count
      NikonKey = fgetc(ifp)^fgetc(ifp)^fgetc(ifp)^fgetc(ifp);
      if ( (unsigned) (ver97-200) < 17) {
        ci = xlat[0][serial & 0xff];
        cj = xlat[1][NikonKey];
        ck = 0x60;
        for (i=0; i < 324; i++)
          buf97[i] ^= (cj += ci * ck++);
        i = "66666>666;6A;:;55"[ver97-200] - '0';
        FORC4 cam_mul[c ^ (c >> 1) ^ (i & 1)] =
          sget2 (buf97 + (i & -2) + c*2);
      }
#ifdef LIBRAW_LIBRARY_BUILD
      if ((NikonLensDataVersion > 200) && lenNikonLensData)
      {
        ci = xlat[0][serial & 0xff];
        cj = xlat[1][NikonKey];
        ck = 0x60;
        for (i = 0; i < lenNikonLensData; i++)
          table_buf[i] ^= (cj += ci * ck++);
        processNikonLensData(table_buf, lenNikonLensData);
        lenNikonLensData = 0;
        free(table_buf);
      }
      if (ver97 == 601)  // Coolpix A
    	{
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
          imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
    	}
#endif
    }

    if(tag == 0xb001 && type == 3)	// Sony ModelID
      {
        unique_id = get2();
      }
    if (tag == 0x200 && len == 3)
      shot_order = (get4(),get4());
    if (tag == 0x200 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2();
    if (tag == 0x201 && len == 4)
         FORC4 cam_mul[c ^ (c >> 1)] = get2();
    if (tag == 0x220 && type == 7)
      meta_offset = ftell(ifp);
    if (tag == 0x401 && type == 4 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get4();
#ifdef LIBRAW_LIBRARY_BUILD
    // not corrected for file bitcount, to be patched in open_datastream
    if (tag == 0x03d && strstr(make,"NIKON") && len == 4)
      {
        FORC4 cblack[c ^ c >> 1] = get2();
        i = cblack[3];
        FORC3 if(i>cblack[c]) i = cblack[c];
        FORC4 cblack[c]-=i;
        black += i;
      }
#endif
    if (tag == 0xe01) {		/* Nikon Capture Note */
#ifdef LIBRAW_LIBRARY_BUILD
	int loopc = 0;
#endif
      order = 0x4949;
      fseek (ifp, 22, SEEK_CUR);
      for (offset=22; offset+22 < len; offset += 22+i) {
#ifdef LIBRAW_LIBRARY_BUILD
	if(loopc++>1024)
		throw LIBRAW_EXCEPTION_IO_CORRUPT;
#endif
	tag = get4();
	fseek (ifp, 14, SEEK_CUR);
	i = get4()-4;
	if (tag == 0x76a43207) flip = get2();
	else fseek (ifp, i, SEEK_CUR);
      }
    }
    if (tag == 0xe80 && len == 256 && type == 7) {
      fseek (ifp, 48, SEEK_CUR);
      cam_mul[0] = get2() * 508 * 1.078 / 0x10000;
      cam_mul[2] = get2() * 382 * 1.173 / 0x10000;
    }
    if (tag == 0xf00 && type == 7) {
      if (len == 614)
	fseek (ifp, 176, SEEK_CUR);
      else if (len == 734 || len == 1502)
	fseek (ifp, 148, SEEK_CUR);
      else goto next;
      goto get2_256;
    }
    if ((tag == 0x1011 && len == 9) || tag == 0x20400200)
      {
        if(!strncasecmp(make,"Olympus", 7))
          {
            int j,k;
            for (i=0; i < 3; i++)
              FORC3 adobe_cam[i][c] = ((short) get2()) / 256.0;
            for (i=0; i < 3; i++)
              for (j=0; j < 3; j++)
                for (cmatrix[i][j] = k=0; k < 3; k++)
                  cmatrix[i][j] += rgb_adobe[i][k] * adobe_cam[k][j];
          }
        else
          for (i=0; i < 3; i++)
            FORC3 cmatrix[i][c] = ((short) get2()) / 256.0;
      }
    if ((tag == 0x1012 || tag == 0x20400600) && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2();
    if (tag == 0x1017 || tag == 0x20400100)
      cam_mul[0] = get2() / 256.0;
    if (tag == 0x1018 || tag == 0x20400100)
      cam_mul[2] = get2() / 256.0;
    if (tag == 0x2011 && len == 2) {
get2_256:
      order = 0x4d4d;
      cam_mul[0] = get2() / 256.0;
      cam_mul[2] = get2() / 256.0;
    }
    if ((tag | 0x70) == 0x2070 && (type == 4 || type == 13))
      fseek (ifp, get4()+base, SEEK_SET);
    if ((tag == 0x2020) && ((type == 7) || (type == 13)))
      parse_thumb_note (base, 257, 258);
    if (tag == 0x2040)
      parse_makernote (base, 0x2040);
#ifdef LIBRAW_LIBRARY_BUILD
// IB start
    if (tag == 0x2010)
      {
        INT64 _pos3 = ftell(ifp);
        parse_makernote(base, 0x2010);
        fseek(ifp,_pos3,SEEK_SET);
      }
// IB end
#endif
    if (tag == 0xb028) {
      fseek (ifp, get4()+base, SEEK_SET);
      parse_thumb_note (base, 136, 137);
    }
    if (tag == 0x4001 && len > 500 && len < 100000) {
      i = len == 582 ? 50 : len == 653 ? 68 : len == 5120 ? 142 : 126;
      fseek (ifp, i, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
      for (i+=18; i <= len; i+=10) {
      	get2();
      	FORC4 sraw_mul[c ^ (c >> 1)] = get2();
      	if (sraw_mul[1] == 1170) break;
      }
    }
    if(!strncasecmp(make,"Samsung",7))
      {
        if (tag == 0xa020) // get the full Samsung encryption key
            for (i=0; i<11; i++) SamsungKey[i] = get4();
        if (tag == 0xa021) // get and decode Samsung cam_mul array
            FORC4 cam_mul[c ^ (c >> 1)] = get4() - SamsungKey[c];
        if (tag == 0xa030 && len == 9)	// get and decode Samsung color matrix
            for (i=0; i < 3; i++)
              FORC3 cmatrix[i][c] = (short)((get4() + SamsungKey[i*3+c]))/256.0;
        if (tag == 0xa028)
          FORC4 cblack[c ^ (c >> 1)] = get4() - SamsungKey[c];
      }
    else
      {
        // Somebody else use 0xa021 and 0xa028?
        if (tag == 0xa021)
          FORC4 cam_mul[c ^ (c >> 1)] = get4();
        if (tag == 0xa028)
          FORC4 cam_mul[c ^ (c >> 1)] -= get4();
      }
    if (tag == 0x4021 && get4() && get4())
      FORC4 cam_mul[c] = 1024;
next:
    fseek (ifp, save, SEEK_SET);
  }
quit:
  order = sorder;
}