int CLASS parse_tiff_ifd(int base)
{
  unsigned entries, tag, type, len, plen = 16, save;
  int ifd, use_cm = 0, cfa, i, j, c, ima_len = 0;
  char *cbuf, *cp;
  uchar cfa_pat[16], cfa_pc[] = {0, 1, 2, 3}, tab[256];
  double fm[3][4], cc[4][4], cm[4][3], cam_xyz[4][3], num;
  double ab[] = {1, 1, 1, 1}, asn[] = {0, 0, 0, 0}, xyz[] = {1, 1, 1};
  unsigned sony_curve[] = {0, 0, 0, 0, 0, 4095};
  unsigned *buf, sony_offset = 0, sony_length = 0, sony_key = 0;
  struct jhead jh;
  int pana_raw = 0;
#ifndef LIBRAW_LIBRARY_BUILD
  FILE *sfp;
#endif

  if (tiff_nifds >= sizeof tiff_ifd / sizeof tiff_ifd[0])
    return 1;
  ifd = tiff_nifds++;
  for (j = 0; j < 4; j++)
    for (i = 0; i < 4; i++)
      cc[j][i] = i == j;
  entries = get2();
  if (entries > 512)
    return 1;
#ifdef LIBRAW_LIBRARY_BUILD
  INT64 fsize = ifp->size();
#endif

  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
#ifdef LIBRAW_LIBRARY_BUILD
    INT64 savepos = ftell(ifp);
    if (len > 8 && savepos + len > 2 * fsize)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    if (callbacks.exif_cb)
    {
      callbacks.exif_cb(callbacks.exifparser_data, tag | (pana_raw ? 0x30000 : ((ifd + 1) << 20)),
                        type, len, order, ifp,base);
      fseek(ifp, savepos, SEEK_SET);
    }
#endif
    switch (tag)
    {
    case 1:
      if (len == 4)
        pana_raw = get4();
      break;
    case 5:
      width = get2();
      break;
    case 6:
      height = get2();
      break;
    case 7:
      width += get2();
      break;
    case 9:
      if ((i = get2()))
        filters = i;
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 10:
      if (pana_raw && len == 1 && type == 3)
      {
        pana_bpp = get2();
      }
    break;
#endif
    case 14:
    case 15:
    case 16:
#ifdef LIBRAW_LIBRARY_BUILD
      if (pana_raw)
      {
        imgdata.color.linear_max[tag - 14] = get2();
        if (tag == 15)
          imgdata.color.linear_max[3] = imgdata.color.linear_max[1];
      }
#endif
      break;
    case 17:
    case 18:
      if (type == 3 && len == 1)
        cam_mul[(tag - 17) * 2] = get2() / 256.0;
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 19:
      if (pana_raw)
      {
        ushort nWB, cnt, tWB;
        nWB = get2();
        if (nWB > 0x100)
          break;
        for (cnt = 0; cnt < nWB; cnt++)
        {
          tWB = get2();
          if (tWB < 0x100)
          {
            imgdata.color.WB_Coeffs[tWB][0] = get2();
            imgdata.color.WB_Coeffs[tWB][2] = get2();
            imgdata.color.WB_Coeffs[tWB][1] = imgdata.color.WB_Coeffs[tWB][3] = 0x100;
          }
          else
            get4();
        }
      }
      break;

    case 0x0120:
      if (pana_raw)
      {
        unsigned sorder = order;
        unsigned long sbase = base;
        base = ftell(ifp);
        order = get2();
        fseek(ifp, 2, SEEK_CUR);
        fseek(ifp, get4()-8, SEEK_CUR);
        parse_tiff_ifd (base);
        base = sbase;
        order = sorder;
      }
    break;
    case 0x0121:
      if (pana_raw)
      { /* 0 is Off, 65536 is Pixel Shift */
        imgdata.makernotes.panasonic.Multishot = get4();
      }
    break;
    case 0x2009:
      if ((pana_encoding == 4) || (pana_encoding == 5))
      {
        int n = MIN (8, len);
        int permut[8] = {3, 2, 1, 0, 3+4, 2+4, 1+4, 0+4};

        imgdata.makernotes.panasonic.BlackLevelDim = len;

        for (int i=0; i < n; i++)
        {
          imgdata.makernotes.panasonic.BlackLevel[permut[i]] =
            (float) (get2()) / (float) (powf(2.f, 14.f-pana_bpp));
        }
      }
      break;
#endif

    case 23:
      if (type == 3)
        iso_speed = get2();
      break;
    case 28:
    case 29:
    case 30:
#ifdef LIBRAW_LIBRARY_BUILD
      if (pana_raw && len == 1 && type == 3)
      {
        pana_black[tag - 28] = get2();
      }
      else
#endif
      {
        cblack[tag - 28] = get2();
        cblack[3] = cblack[1];
      }
      break;
    case 36:
    case 37:
    case 38:
      cam_mul[tag - 36] = get2();
      break;
    case 39:
#ifdef LIBRAW_LIBRARY_BUILD
      if (pana_raw)
      {
        ushort nWB, cnt, tWB;
        nWB = get2();
        if (nWB > 0x100)
          break;
        for (cnt = 0; cnt < nWB; cnt++)
        {
          tWB = get2();
          if (tWB < 0x100)
          {
            imgdata.color.WB_Coeffs[tWB][0] = get2();
            imgdata.color.WB_Coeffs[tWB][1] = imgdata.color.WB_Coeffs[tWB][3] = get2();
            imgdata.color.WB_Coeffs[tWB][2] = get2();
          }
          else
            fseek(ifp, 6, SEEK_CUR);
        }
      }
      break;
#endif
      if (len < 50 || cam_mul[0]>0.001f)
        break;
      fseek(ifp, 12, SEEK_CUR);
      FORC3 cam_mul[c] = get2();
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 45:
      if (pana_raw && len == 1 && type == 3)
      {
        pana_encoding = get2();
      }
      break;
#endif
    case 46:
      if (type != 7 || fgetc(ifp) != 0xff || fgetc(ifp) != 0xd8)
        break;
      thumb_offset = ftell(ifp) - 2;
      thumb_length = len;
      break;
    case 61440: /* Fuji HS10 table */
      fseek(ifp, get4() + base, SEEK_SET);
      parse_tiff_ifd(base);
      break;
    case 2:  /* ImageWidth */
    case 256:
    case 61441: /* Fuji RAF IFD 0xf001 RawImageFullWidth */
      tiff_ifd[ifd].t_width = getint(type);
      break;
    case 3: /* ImageHeight */
    case 257:
    case 61442: /* Fuji RAF IFD 0xf002 RawImageFullHeight */
      tiff_ifd[ifd].t_height = getint(type);
      break;
    case 258:   /* BitsPerSample */
    case 61443: /* Fuji RAF IFD 0xf003 */
      tiff_ifd[ifd].samples = len & 7;
      tiff_ifd[ifd].bps = getint(type);
      if (tiff_bps < tiff_ifd[ifd].bps)
        tiff_bps = tiff_ifd[ifd].bps;
      break;
    case 61446: /* Fuji RAF IFD 0xf006 */
      raw_height = 0;
      if (tiff_ifd[ifd].bps > 12)
        break;
      load_raw = &CLASS packed_load_raw;
      load_flags = get4() ? 24 : 80;
      break;
    case 259: /* Compression */
      tiff_ifd[ifd].comp = getint(type);
      break;
    case 262: /* PhotometricInterpretation */
      tiff_ifd[ifd].phint = get2();
      break;
    case 270: /* ImageDescription */
      fread(desc, 512, 1, ifp);
      break;
    case 271: /* Make */
      fgets(make, 64, ifp);
      break;
    case 272: /* Model */
      fgets(model, 64, ifp);
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 278:
      tiff_ifd[ifd].rows_per_strip = getint(type);
      break;
#endif
    case 280: /* Panasonic RW2 offset */
      if (type != 4)
        break;
      load_raw = &CLASS panasonic_load_raw;
      load_flags = 0x2008;
    case 273: /* StripOffset */
#ifdef LIBRAW_LIBRARY_BUILD
      if (len > 1 && len < 16384)
      {
        off_t sav = ftell(ifp);
        tiff_ifd[ifd].strip_offsets = (int *)calloc(len, sizeof(int));
        tiff_ifd[ifd].strip_offsets_count = len;
        for (int i = 0; i < len; i++)
          tiff_ifd[ifd].strip_offsets[i] = get4() + base;
        fseek(ifp, sav, SEEK_SET); // restore position
      }
/* fallback */
#endif
    case 513: /* JpegIFOffset */
    case 61447:
      tiff_ifd[ifd].offset = get4() + base;
      if (!tiff_ifd[ifd].bps && tiff_ifd[ifd].offset > 0)
      {
        fseek(ifp, tiff_ifd[ifd].offset, SEEK_SET);
        if (ljpeg_start(&jh, 1))
        {
          tiff_ifd[ifd].comp = 6;
          tiff_ifd[ifd].t_width = jh.wide;
          tiff_ifd[ifd].t_height = jh.high;
          tiff_ifd[ifd].bps = jh.bits;
          tiff_ifd[ifd].samples = jh.clrs;
          if (!(jh.sraw || (jh.clrs & 1)))
            tiff_ifd[ifd].t_width *= jh.clrs;
          if ((tiff_ifd[ifd].t_width > 4 * tiff_ifd[ifd].t_height) & ~jh.clrs)
          {
            tiff_ifd[ifd].t_width /= 2;
            tiff_ifd[ifd].t_height *= 2;
          }
          i = order;
          parse_tiff(tiff_ifd[ifd].offset + 12);
          order = i;
        }
      }
      break;
    case 274: /* Orientation */
      tiff_ifd[ifd].t_flip = "50132467"[get2() & 7] - '0';
      break;
    case 277: /* SamplesPerPixel */
      tiff_ifd[ifd].samples = getint(type) & 7;
      break;
    case 279: /* StripByteCounts */
#ifdef LIBRAW_LIBRARY_BUILD
      if (len > 1 && len < 16384)
      {
        off_t sav = ftell(ifp);
        tiff_ifd[ifd].strip_byte_counts = (int *)calloc(len, sizeof(int));
        tiff_ifd[ifd].strip_byte_counts_count = len;
        for (int i = 0; i < len; i++)
          tiff_ifd[ifd].strip_byte_counts[i] = get4();
        fseek(ifp, sav, SEEK_SET); // restore position
      }
/* fallback */
#endif
    case 514:
    case 61448:
      tiff_ifd[ifd].bytes = get4();
      break;
    case 61454: // FujiFilm "As Shot"
      FORC3 cam_mul[(4 - c) % 3] = getint(type);
      break;
    case 305:
    case 11: /* Software */
      if ((pana_raw) && (tag == 11) && (type == 3))
      {
#ifdef LIBRAW_LIBRARY_BUILD
        imgdata.makernotes.panasonic.Compression = get2();
#endif
        break;
      }
      fgets(software, 64, ifp);
      if (!strncmp(software, "Adobe", 5) || !strncmp(software, "dcraw", 5) || !strncmp(software, "UFRaw", 5) ||
          !strncmp(software, "Bibble", 6) || !strcmp(software, "Digital Photo Professional"))
        is_raw = 0;
      break;
    case 306: /* DateTime */
      get_timestamp(0);
      break;
    case 315: /* Artist */
      fread(artist, 64, 1, ifp);
      break;
    case 317:
      tiff_ifd[ifd].predictor = getint(type);
      break;
    case 322: /* TileWidth */
      tiff_ifd[ifd].t_tile_width = getint(type);
      break;
    case 323: /* TileLength */
      tiff_ifd[ifd].t_tile_length = getint(type);
      break;
    case 324: /* TileOffsets */
      tiff_ifd[ifd].offset = len > 1 ? ftell(ifp) : get4();
      if (len == 1)
        tiff_ifd[ifd].t_tile_width = tiff_ifd[ifd].t_tile_length = 0;
      if (len == 4)
      {
        load_raw = &CLASS sinar_4shot_load_raw;
        is_raw = 5;
      }
      break;
    case 325:
      tiff_ifd[ifd].bytes = len > 1 ? ftell(ifp) : get4();
      break;
    case 330: /* SubIFDs */
      if (!strcmp(model, "DSLR-A100") && tiff_ifd[ifd].t_width == 3872)
      {
        load_raw = &CLASS sony_arw_load_raw;
        data_offset = get4() + base;
        ifd++;
#ifdef LIBRAW_LIBRARY_BUILD
        if (ifd >= sizeof tiff_ifd / sizeof tiff_ifd[0])
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
#endif
        break;
      }
#ifdef LIBRAW_LIBRARY_BUILD
      if (!strncmp(make, "Hasselblad", 10) && libraw_internal_data.unpacker_data.hasselblad_parser_flag)
      {
        fseek(ifp, ftell(ifp) + 4, SEEK_SET);
        fseek(ifp, get4() + base, SEEK_SET);
        parse_tiff_ifd(base);
        break;
      }
#endif
      if (len > 1000)
        len = 1000; /* 1000 SubIFDs is enough */
      while (len--)
      {
        i = ftell(ifp);
        fseek(ifp, get4() + base, SEEK_SET);
        if (parse_tiff_ifd(base))
          break;
        fseek(ifp, i + 4, SEEK_SET);
      }
      break;
    case 339:
      tiff_ifd[ifd].sample_format = getint(type);
      break;
    case 400:
      strcpy(make, "Sarnoff");
      maximum = 0xfff;
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 700:
      if ((type == 1 || type == 2 || type == 6 || type == 7) && len > 1 && len < 5100000)
      {
        xmpdata = (char *)malloc(xmplen = len + 1);
        fread(xmpdata, len, 1, ifp);
        xmpdata[len] = 0;
      }
      break;
    case 0x7000:
      imgdata.makernotes.sony.SonyRawFileType = get2();
      break;
#endif
    case 28688:
      FORC4 sony_curve[c + 1] = get2() >> 2 & 0xfff;
      for (i = 0; i < 5; i++)
        for (j = sony_curve[i] + 1; j <= sony_curve[i + 1]; j++)
          curve[j] = curve[j - 1] + (1 << i);
      break;
    case 29184:  // Sony SR2Private 0x7200
      sony_offset = get4();
      break;
    case 29185:  // Sony SR2Private 0x7201
      sony_length = get4();
      break;
    case 29217:  // Sony SR2Private 0x7221
      sony_key = get4();
      break;
    case 29264:  // Sony SR2Private 0x7250
      parse_minolta(ftell(ifp));
      raw_width = 0;
      break;
    case 29443:  // Sony SR2SubIFD 0x7303
      FORC4 cam_mul[c ^ (c < 2)] = get2();
      break;
    case 29459:  // Sony SR2SubIFD 0x7313
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
      break;
    case 29456:  // Sony SR2SubIFD 0x7310
      FORC4 cblack[c ^ c >> 1] = get2();
      i = cblack[3];
      FORC3 if (i > cblack[c]) i = cblack[c];
      FORC4 cblack[c] -= i;
      black = i;

#ifdef DCRAW_VERBOSE
      if (verbose)
        fprintf(stderr, _("...Sony black: %u cblack: %u %u %u %u\n"), black, cblack[0], cblack[1], cblack[2],
                cblack[3]);
#endif
      break;
    case 33405: /* Model2 */
      fgets(model2, 64, ifp);
      break;
    case 33421: /* CFARepeatPatternDim */
      if (get2() == 6 && get2() == 6)
        filters = 9;
      break;
    case 33422: /* CFAPattern */
      if (filters == 9)
      {
        FORC(36)((char *)xtrans)[c] = fgetc(ifp) & 3;
        break;
      }
    case 64777: /* Kodak P-series */
      if (len == 36)
      {
        filters = 9;
        colors = 3;
        FORC(36) xtrans[0][c] = fgetc(ifp) & 3;
      }
      else if (len > 0)
      {
        if ((plen = len) > 16)
          plen = 16;
        fread(cfa_pat, 1, plen, ifp);
        for (colors = cfa = i = 0; i < plen && colors < 4; i++)
        {
	  if(cfa_pat[i] > 31) continue; // Skip wrong data
          colors += !(cfa & (1 << cfa_pat[i]));
          cfa |= 1 << cfa_pat[i];
        }
        if (cfa == 070)
          memcpy(cfa_pc, "\003\004\005", 3); /* CMY */
        if (cfa == 072)
          memcpy(cfa_pc, "\005\003\004\001", 4); /* GMCY */
        goto guess_cfa_pc;
      }
      break;
    case 33424:
    case 65024:
      fseek(ifp, get4() + base, SEEK_SET);
      parse_kodak_ifd(base);
      break;
    case 33434: /* ExposureTime */
      tiff_ifd[ifd].t_shutter = shutter = getreal(type);
      break;
    case 33437: /* FNumber */
      aperture = getreal(type);
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    // IB start
    case 0x9400:
      imgdata.other.exifAmbientTemperature = getreal(type);
      if ((imgdata.other.CameraTemperature > -273.15f) && (OlyID == 0x4434353933ULL)) // TG-5
        imgdata.other.CameraTemperature += imgdata.other.exifAmbientTemperature;
      break;
    case 0x9401:
      imgdata.other.exifHumidity = getreal(type);
      break;
    case 0x9402:
      imgdata.other.exifPressure = getreal(type);
      break;
    case 0x9403:
      imgdata.other.exifWaterDepth = getreal(type);
      break;
    case 0x9404:
      imgdata.other.exifAcceleration = getreal(type);
      break;
    case 0x9405:
      imgdata.other.exifCameraElevationAngle = getreal(type);
      break;
    case 0xa405: // FocalLengthIn35mmFormat
      imgdata.lens.FocalLengthIn35mmFormat = get2();
      break;
    case 0xa431: // BodySerialNumber
    case 0xc62f:
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
      break;
    case 0xa432: // LensInfo, 42034dec, Lens Specification per EXIF standard
      imgdata.lens.MinFocal = getreal(type);
      imgdata.lens.MaxFocal = getreal(type);
      imgdata.lens.MaxAp4MinFocal = getreal(type);
      imgdata.lens.MaxAp4MaxFocal = getreal(type);
      break;
    case 0xa435: // LensSerialNumber
      stmread(imgdata.lens.LensSerial, len, ifp);
      break;
    case 0xc630: // DNG LensInfo, Lens Specification per EXIF standard
      imgdata.lens.MinFocal = getreal(type);
      imgdata.lens.MaxFocal = getreal(type);
      imgdata.lens.MaxAp4MinFocal = getreal(type);
      imgdata.lens.MaxAp4MaxFocal = getreal(type);
      break;
    case 0xa433: // LensMake
      stmread(imgdata.lens.LensMake, len, ifp);
      break;
    case 0xa434: // LensModel
      stmread(imgdata.lens.Lens, len, ifp);
      if (!strncmp(imgdata.lens.Lens, "----", 4))
        imgdata.lens.Lens[0] = 0;
      break;
    case 0x9205:
      imgdata.lens.EXIF_MaxAp = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      break;
// IB end
#endif
    case 34306: /* Leaf white balance */
      FORC4
      {
      	int q = get2();
	if(q)
      		cam_mul[c ^ 1] = 4096.0 / q;
      }
      break;
    case 34307: /* Leaf CatchLight color matrix */
      fread(software, 1, 7, ifp);
      if (strncmp(software, "MATRIX", 6))
        break;
      colors = 4;
      for (raw_color = i = 0; i < 3; i++)
      {
        FORC4 fscanf(ifp, "%f", &rgb_cam[i][c ^ 1]);
        if (!use_camera_wb)
          continue;
        num = 0;
        FORC4 num += rgb_cam[i][c];
        FORC4 rgb_cam[i][c] /= MAX(1, num);
      }
      break;
    case 34310: /* Leaf metadata */
      parse_mos(ftell(ifp));
    case 34303:
      strcpy(make, "Leaf");
      break;
    case 34665: /* EXIF tag */
      fseek(ifp, get4() + base, SEEK_SET);
      parse_exif(base);
      break;
    case 34853: /* GPSInfo tag */
    {
      unsigned pos;
      fseek(ifp, pos = (get4() + base), SEEK_SET);
      parse_gps(base);
#ifdef LIBRAW_LIBRARY_BUILD
      fseek(ifp, pos, SEEK_SET);
      parse_gps_libraw(base);
#endif
    }
    break;
    case 34675: /* InterColorProfile */
    case 50831: /* AsShotICCProfile */
      profile_offset = ftell(ifp);
      profile_length = len;
      break;
    case 37122: /* CompressedBitsPerPixel */
      kodak_cbpp = get4();
      break;
    case 37386: /* FocalLength */
      focal_len = getreal(type);
      break;
    case 37393: /* ImageNumber */
      shot_order = getint(type);
      break;
    case 37400: /* old Kodak KDC tag */
      for (raw_color = i = 0; i < 3; i++)
      {
        getreal(type);
        FORC3 rgb_cam[i][c] = getreal(type);
      }
      break;
    case 40976:
      strip_offset = get4();
      switch (tiff_ifd[ifd].comp)
      {
      case 32770:
        load_raw = &CLASS samsung_load_raw;
        break;
      case 32772:
        load_raw = &CLASS samsung2_load_raw;
        break;
      case 32773:
        load_raw = &CLASS samsung3_load_raw;
        break;
      }
      break;
    case 46275: /* Imacon tags */
      strcpy(make, "Imacon");
      data_offset = ftell(ifp);
      ima_len = len;
      break;
    case 46279:
      if (!ima_len)
        break;
      fseek(ifp, 38, SEEK_CUR);
    case 46274:
      fseek(ifp, 40, SEEK_CUR);
      raw_width = get4();
      raw_height = get4();
      left_margin = get4() & 7;
      width = raw_width - left_margin - (get4() & 7);
      top_margin = get4() & 7;
      height = raw_height - top_margin - (get4() & 7);
      if (raw_width == 7262 && ima_len == 234317952)
      {
        height = 5412;
        width = 7216;
        left_margin = 7;
        filters = 0;
      }
      else if (raw_width == 7262)
      {
        height = 5444;
        width = 7244;
        left_margin = 7;
      }
      fseek(ifp, 52, SEEK_CUR);
      FORC3 cam_mul[c] = getreal(11);
      fseek(ifp, 114, SEEK_CUR);
      flip = (get2() >> 7) * 90;
      if (width * height * 6 == ima_len)
      {
        if (flip % 180 == 90)
          SWAP(width, height);
        raw_width = width;
        raw_height = height;
        left_margin = top_margin = filters = flip = 0;
      }
      sprintf(model, "Ixpress %d-Mp", height * width / 1000000);
      load_raw = &CLASS imacon_full_load_raw;
      if (filters)
      {
        if (left_margin & 1)
          filters = 0x61616161;
        load_raw = &CLASS unpacked_load_raw;
      }
      maximum = 0xffff;
      break;
    case 50454: /* Sinar tag */
    case 50455:
      if (len < 1 || len > 2560000 || !(cbuf = (char *)malloc(len)))
        break;
#ifndef LIBRAW_LIBRARY_BUILD
      fread(cbuf, 1, len, ifp);
#else
      if (fread(cbuf, 1, len, ifp) != len)
        throw LIBRAW_EXCEPTION_IO_CORRUPT; // cbuf to be free'ed in recycle
#endif
      cbuf[len - 1] = 0;
      for (cp = cbuf - 1; cp && cp < cbuf + len; cp = strchr(cp, '\n'))
        if (!strncmp(++cp, "Neutral ", 8))
          sscanf(cp + 8, "%f %f %f", cam_mul, cam_mul + 1, cam_mul + 2);
      free(cbuf);
      break;
    case 50458:
      if (!make[0])
        strcpy(make, "Hasselblad");
      break;
    case 50459: /* Hasselblad tag */
#ifdef LIBRAW_LIBRARY_BUILD
      libraw_internal_data.unpacker_data.hasselblad_parser_flag = 1;
#endif
      i = order;
      j = ftell(ifp);
      c = tiff_nifds;
      order = get2();
      fseek(ifp, j + (get2(), get4()), SEEK_SET);
      parse_tiff_ifd(j);
      maximum = 0xffff;
      tiff_nifds = c;
      order = i;
      break;
    case 50706: /* DNGVersion */
      FORC4 dng_version = (dng_version << 8) + fgetc(ifp);
      if (!make[0])
        strcpy(make, "DNG");
      is_raw = 1;
      break;
    case 50708: /* UniqueCameraModel */
#ifdef LIBRAW_LIBRARY_BUILD
      stmread(imgdata.color.UniqueCameraModel, len, ifp);
      imgdata.color.UniqueCameraModel[sizeof(imgdata.color.UniqueCameraModel) - 1] = 0;
#endif
      if (model[0])
        break;
#ifndef LIBRAW_LIBRARY_BUILD
      fgets(make, 64, ifp);
#else
      strncpy(make, imgdata.color.UniqueCameraModel, MIN(len, sizeof(imgdata.color.UniqueCameraModel)));
#endif
      if ((cp = strchr(make, ' ')))
      {
        strcpy(model, cp + 1);
        *cp = 0;
      }
      break;
    case 50710: /* CFAPlaneColor */
      if (filters == 9)
        break;
      if (len > 4)
        len = 4;
      colors = len;
      fread(cfa_pc, 1, colors, ifp);
    guess_cfa_pc:
      FORCC tab[cfa_pc[c]] = c;
      cdesc[c] = 0;
      for (i = 16; i--;)
        filters = filters << 2 | tab[cfa_pat[i % plen]];
      filters -= !filters;
      break;
    case 50711: /* CFALayout */
      if (get2() == 2)
        fuji_width = 1;
      break;
    case 291:
    case 50712: /* LinearizationTable */
#ifdef LIBRAW_LIBRARY_BUILD
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_LINTABLE;
      tiff_ifd[ifd].lineartable_offset = ftell(ifp);
      tiff_ifd[ifd].lineartable_len = len;
#endif
      linear_table(len);
      break;
    case 50713: /* BlackLevelRepeatDim */
#ifdef LIBRAW_LIBRARY_BUILD
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
      tiff_ifd[ifd].dng_levels.dng_fcblack[4] =
      tiff_ifd[ifd].dng_levels.dng_cblack[4] =
#endif
          cblack[4] = get2();
#ifdef LIBRAW_LIBRARY_BUILD
      tiff_ifd[ifd].dng_levels.dng_fcblack[5] =
      tiff_ifd[ifd].dng_levels.dng_cblack[5] =
#endif
          cblack[5] = get2();
      if (cblack[4] * cblack[5] > (sizeof(cblack) / sizeof(cblack[0]) - 6))
#ifdef LIBRAW_LIBRARY_BUILD
        tiff_ifd[ifd].dng_levels.dng_fcblack[4] = tiff_ifd[ifd].dng_levels.dng_fcblack[5] =
        tiff_ifd[ifd].dng_levels.dng_cblack[4] = tiff_ifd[ifd].dng_levels.dng_cblack[5] =
#endif
            cblack[4] = cblack[5] = 1;
      break;

#ifdef LIBRAW_LIBRARY_BUILD
    case 0xf00d:
      if (!is_4K_RAFdata)
      {
        FORC3 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][(4 - c) % 3] = getint(type);
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][3] = imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][1];
      }
      break;
    case 0xf00c:
      if (!is_4K_RAFdata)
      {
        unsigned fwb[4];
        FORC4 fwb[c] = get4();
        if (fwb[3] < 0x100)
        {
          imgdata.color.WB_Coeffs[fwb[3]][0] = fwb[1];
          imgdata.color.WB_Coeffs[fwb[3]][1] = imgdata.color.WB_Coeffs[fwb[3]][3] = fwb[0];
          imgdata.color.WB_Coeffs[fwb[3]][2] = fwb[2];
          if ((fwb[3] == 17) && (libraw_internal_data.unpacker_data.lenRAFData > 3) &&
              (libraw_internal_data.unpacker_data.lenRAFData < 10240000))
          {
            INT64 f_save = ftell(ifp);
            ushort *rafdata = (ushort *)malloc(sizeof(ushort) * libraw_internal_data.unpacker_data.lenRAFData);
            fseek(ifp, libraw_internal_data.unpacker_data.posRAFData, SEEK_SET);
            fread(rafdata, sizeof(ushort), libraw_internal_data.unpacker_data.lenRAFData, ifp);
            fseek(ifp, f_save, SEEK_SET);
            int fj, found = 0;
            for (int fi = 0; fi < (libraw_internal_data.unpacker_data.lenRAFData - 3); fi++)
            {
              if ((fwb[0] == rafdata[fi]) && (fwb[1] == rafdata[fi + 1]) && (fwb[2] == rafdata[fi + 2]))
              {
                if (rafdata[fi - 15] != fwb[0])
                  continue;

                for (int wb_ind = 0, ofst = fi - 15; wb_ind < nFuji_wb_list1; wb_ind++, ofst += 3)
                {
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][1] =
                      imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][3] = rafdata[ofst];
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][0] = rafdata[ofst + 1];
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][2] = rafdata[ofst + 2];
                }
                fi += 0x60;
                for (fj = fi; fj < (fi + 15); fj += 3)
                  if (rafdata[fj] != rafdata[fi])
                  {
                    found = 1;
                    break;
                  }
                if (found)
                {
                  fj = fj - 93;
                  for (int iCCT = 0; iCCT < 31; iCCT++)
                  {
                    imgdata.color.WBCT_Coeffs[iCCT][0] = FujiCCT_K[iCCT];
                    imgdata.color.WBCT_Coeffs[iCCT][1] = rafdata[iCCT * 3 + 1 + fj];
                    imgdata.color.WBCT_Coeffs[iCCT][2] = imgdata.color.WBCT_Coeffs[iCCT][4] = rafdata[iCCT * 3 + fj];
                    imgdata.color.WBCT_Coeffs[iCCT][3] = rafdata[iCCT * 3 + 2 + fj];
                  }
                }
                free(rafdata);
                break;
              }
            }
          }
        }
        FORC4 fwb[c] = get4();
        if (fwb[3] < 0x100)
        {
          imgdata.color.WB_Coeffs[fwb[3]][0] = fwb[1];
          imgdata.color.WB_Coeffs[fwb[3]][1] = imgdata.color.WB_Coeffs[fwb[3]][3] = fwb[0];
          imgdata.color.WB_Coeffs[fwb[3]][2] = fwb[2];
        }
      }
      break;
#endif

#ifdef LIBRAW_LIBRARY_BUILD
    case 50709:
      stmread(imgdata.color.LocalizedCameraModel, len, ifp);
      break;
#endif

    case 61450:
      cblack[4] = cblack[5] = MIN(sqrt((double)len), 64);
    case 50714: /* BlackLevel */
#ifdef LIBRAW_LIBRARY_BUILD
      if (tiff_ifd[ifd].samples > 1 && tiff_ifd[ifd].samples == len) // LinearDNG, per-channel black
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
        for (i = 0; i < 4 && i < len; i++)
	  {
	    tiff_ifd[ifd].dng_levels.dng_fcblack[i] = getreal(type);
	    tiff_ifd[ifd].dng_levels.dng_cblack[i] = cblack[i] = tiff_ifd[ifd].dng_levels.dng_fcblack[i]+0.5;
	  }

        tiff_ifd[ifd].dng_levels.dng_fblack = tiff_ifd[ifd].dng_levels.dng_black = black = 0;
      }
      else
#endif
          if ((cblack[4] * cblack[5] < 2) && len == 1)
      {
#ifdef LIBRAW_LIBRARY_BUILD
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
        tiff_ifd[ifd].dng_levels.dng_fblack = getreal(type);
	black = tiff_ifd[ifd].dng_levels.dng_black = tiff_ifd[ifd].dng_levels.dng_fblack;
#else
            black = getreal(type);
#endif
      }
      else if (cblack[4] * cblack[5] <= len)
      {
        FORC(cblack[4] * cblack[5])
#ifdef LIBRAW_LIBRARY_BUILD
	  {
	    tiff_ifd[ifd].dng_levels.dng_fcblack[6+c] = getreal(type);
	    cblack[6+c] = tiff_ifd[ifd].dng_levels.dng_fcblack[6+c];
	  }
#else
	  cblack[6 + c] = getreal(type);
#endif
        black = 0;
        FORC4
        cblack[c] = 0;

#ifdef LIBRAW_LIBRARY_BUILD
        if (tag == 50714)
        {
          tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
          FORC(cblack[4] * cblack[5])
	    tiff_ifd[ifd].dng_levels.dng_cblack[6 + c] = cblack[6 + c];
          tiff_ifd[ifd].dng_levels.dng_fblack = 0;
          tiff_ifd[ifd].dng_levels.dng_black = 0;
          FORC4
	    tiff_ifd[ifd].dng_levels.dng_fcblack[c] =
	    tiff_ifd[ifd].dng_levels.dng_cblack[c] = 0;
        }
#endif
      }
      break;
    case 50715: /* BlackLevelDeltaH */
    case 50716: /* BlackLevelDeltaV */
      for (num = i = 0; i < len && i < 65536; i++)
        num += getreal(type);
      if(len>0)
	{
        black += num / len + 0.5;
#ifdef LIBRAW_LIBRARY_BUILD
	tiff_ifd[ifd].dng_levels.dng_fblack += num/float(len);
	tiff_ifd[ifd].dng_levels.dng_black += num / len + 0.5;
	tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
#endif
	}
      break;
    case 50717: /* WhiteLevel */
#ifdef LIBRAW_LIBRARY_BUILD
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_WHITE;
      tiff_ifd[ifd].dng_levels.dng_whitelevel[0] =
#endif
          maximum = getint(type);
#ifdef LIBRAW_LIBRARY_BUILD
      if (tiff_ifd[ifd].samples > 1) // Linear DNG case
        for (i = 1; i < 4 && i < len; i++)
          tiff_ifd[ifd].dng_levels.dng_whitelevel[i] = getint(type);
#endif
      break;
    case 50718: /* DefaultScale */
      {
	float q1 = getreal(type);
	float q2 = getreal(type);
	if(q1 > 0.00001f && q2 > 0.00001f)
	 {
      		pixel_aspect = q1/q2;
      		if (pixel_aspect > 0.995 && pixel_aspect < 1.005)
        		pixel_aspect = 1.0;
	 }
      }
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 50719: /* DefaultCropOrigin */
      if (len == 2)
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_CROPORIGIN;
        tiff_ifd[ifd].dng_levels.default_crop[0] = getreal(type);
        tiff_ifd[ifd].dng_levels.default_crop[1] = getreal(type);
        if (!strncasecmp(make, "SONY", 4))
        {
          imgdata.sizes.raw_crop.cleft = tiff_ifd[ifd].dng_levels.default_crop[0];
          imgdata.sizes.raw_crop.ctop = tiff_ifd[ifd].dng_levels.default_crop[1];
        }
      }
      break;
    case 50720: /* DefaultCropSize */
      if (len == 2)
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_CROPSIZE;
        tiff_ifd[ifd].dng_levels.default_crop[2] = getreal(type);
        tiff_ifd[ifd].dng_levels.default_crop[3] = getreal(type);
        if (!strncasecmp(make, "SONY", 4))
        {
          imgdata.sizes.raw_crop.cwidth = tiff_ifd[ifd].dng_levels.default_crop[2];
          imgdata.sizes.raw_crop.cheight = tiff_ifd[ifd].dng_levels.default_crop[3];
        }
      }
      break;
#define imSony imgdata.makernotes.sony
    case 0x74c7:
      if ((len == 2) && !strncasecmp(make, "SONY", 4))
      {
        imSony.raw_crop.cleft = get4();
        imSony.raw_crop.ctop = get4();
      }
      break;
    case 0x74c8:
      if ((len == 2) && !strncasecmp(make, "SONY", 4))
      {
        imSony.raw_crop.cwidth = get4();
        imSony.raw_crop.cheight = get4();
      }
      break;
#undef imSony
#endif
#ifdef LIBRAW_LIBRARY_BUILD
    case 50778:
      tiff_ifd[ifd].dng_color[0].illuminant = get2();
      tiff_ifd[ifd].dng_color[0].parsedfields |= LIBRAW_DNGFM_ILLUMINANT;
      break;
    case 50779:
      tiff_ifd[ifd].dng_color[1].illuminant = get2();
      tiff_ifd[ifd].dng_color[1].parsedfields |= LIBRAW_DNGFM_ILLUMINANT;
      break;

#endif
    case 50721: /* ColorMatrix1 */
    case 50722: /* ColorMatrix2 */
    {
      int chan = (len == 9)? 3 : (len == 12?4:0);
#ifdef LIBRAW_LIBRARY_BUILD
      i = tag == 50721 ? 0 : 1;
      if(chan)
         tiff_ifd[ifd].dng_color[i].parsedfields |= LIBRAW_DNGFM_COLORMATRIX;
#endif
      FORC(chan) for (j = 0; j < 3; j++)
      {
#ifdef LIBRAW_LIBRARY_BUILD
        tiff_ifd[ifd].dng_color[i].colormatrix[c][j] =
#endif
            cm[c][j] = getreal(type);
      }
      use_cm = 1;
    }
    break;

    case 0xc714: /* ForwardMatrix1 */
    case 0xc715: /* ForwardMatrix2 */
    {
      int chan = (len == 9)? 3 : (len == 12?4:0);
#ifdef LIBRAW_LIBRARY_BUILD
      i = tag == 0xc714 ? 0 : 1;
      if(chan)
         tiff_ifd[ifd].dng_color[i].parsedfields |= LIBRAW_DNGFM_FORWARDMATRIX;
#endif
      for (j = 0; j < 3; j++)
        FORC(chan)
        {
#ifdef LIBRAW_LIBRARY_BUILD
          tiff_ifd[ifd].dng_color[i].forwardmatrix[j][c] =
#endif
              fm[j][c] = getreal(type);
        }
      }
      break;

    case 50723: /* CameraCalibration1 */
    case 50724: /* CameraCalibration2 */
    {
      int chan = (len == 9)? 3 : (len == 16?4:0);
#ifdef LIBRAW_LIBRARY_BUILD
      j = tag == 50723 ? 0 : 1;
      if(chan)
        tiff_ifd[ifd].dng_color[j].parsedfields |= LIBRAW_DNGFM_CALIBRATION;
#endif
      for (i = 0; i < chan; i++)
        FORC(chan)
        {
#ifdef LIBRAW_LIBRARY_BUILD
          tiff_ifd[ifd].dng_color[j].calibration[i][c] =
#endif
              cc[i][c] = getreal(type);
        }
      }
      break;
    case 50727: /* AnalogBalance */
#ifdef LIBRAW_LIBRARY_BUILD
      if(len>=3)
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_ANALOGBALANCE;
#endif
      for(c = 0; c < len && c < 4; c++)
      {
#ifdef LIBRAW_LIBRARY_BUILD
        tiff_ifd[ifd].dng_levels.analogbalance[c] =
#endif
            ab[c] = getreal(type);
      }
      break;
    case 50728: /* AsShotNeutral */
#ifdef LIBRAW_LIBRARY_BUILD
      if(len>=3)
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_ASSHOTNEUTRAL;
      for(c = 0; c < len && c < 4; c++)
         tiff_ifd[ifd].dng_levels.asshotneutral[c] = asn[c] = getreal(type);
#else
      FORCC asn[c] = getreal(type);
#endif
      break;
    case 50729: /* AsShotWhiteXY */
      xyz[0] = getreal(type);
      xyz[1] = getreal(type);
      xyz[2] = 1 - xyz[0] - xyz[1];
      FORC3 xyz[c] /= d65_white[c];
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 50730: /* DNG: 0xc62a BaselineExposure */
		tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BASELINEEXPOSURE;
		tiff_ifd[ifd].dng_levels.baseline_exposure = getreal(type);
      break;
    case 50734: /* DNG: 0xc62e LinearResponseLimit */
		tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_LINEARRESPONSELIMIT;
		tiff_ifd[ifd].dng_levels.LinearResponseLimit = getreal(type);
      break;
#endif
    // IB start
    case 50740: /* tag 0xc634 : DNG Adobe, DNG Pentax, Sony SR2, DNG Private */
#ifdef LIBRAW_LIBRARY_BUILD
  if (!(imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SKIP_MAKERNOTES)) {
      char mbuf[64];
      unsigned short makernote_found = 0;
      INT64 curr_pos, start_pos = ftell(ifp);
      unsigned MakN_order, m_sorder = order;
      unsigned MakN_length;
      unsigned pos_in_original_raw;
      fread(mbuf, 1, 6, ifp);

      if (!strcmp(mbuf, "Adobe")) {
        order = 0x4d4d; // Adobe header is always in "MM" / big endian
        curr_pos = start_pos + 6;
        while (curr_pos + 8 - start_pos <= len) {
          fread(mbuf, 1, 4, ifp);
          curr_pos += 8;

          if (!strncmp(mbuf, "Pano", 4)) { // PanasonicRaw, yes, they use "Pano" as signature
            parseAdobePanoMakernote();
          }

          if (!strncmp(mbuf, "MakN", 4)) {
            makernote_found = 1;
            MakN_length = get4();
            MakN_order = get2();
            pos_in_original_raw = get4();
            order = MakN_order;

            INT64 save_pos = ifp->tell();
            parse_makernote_0xc634(curr_pos + 6 - pos_in_original_raw, 0, AdobeDNG);

            curr_pos = save_pos + MakN_length - 6;
            fseek(ifp, curr_pos, SEEK_SET);

            fread(mbuf, 1, 4, ifp);
            curr_pos += 8;

            if (!strncmp(mbuf, "Pano ", 4)) {
              parseAdobePanoMakernote();
            }

            if (!strncmp(mbuf, "SR2 ", 4)) {
              order = 0x4d4d;
              MakN_length = get4();
              MakN_order = get2();
              pos_in_original_raw = get4();
              order = MakN_order;

              unsigned *buf_SR2;
              unsigned entries, tag, type, len, save;
              unsigned SR2SubIFDOffset = 0;
              unsigned SR2SubIFDLength = 0;
              unsigned SR2SubIFDKey = 0;
              int base = curr_pos + 6 - pos_in_original_raw;
              entries = get2();
              while (entries--) {
                tiff_get(base, &tag, &type, &len, &save);

                if (tag == 0x7200) {
                  SR2SubIFDOffset = get4();
                } else if (tag == 0x7201) {
                  SR2SubIFDLength = get4();
                } else if (tag == 0x7221) {
                  SR2SubIFDKey = get4();
                }
                fseek(ifp, save, SEEK_SET);
              }

              if (SR2SubIFDLength && (SR2SubIFDLength < 10240000) && (buf_SR2 = (unsigned *)malloc(SR2SubIFDLength+1024))) { // 1024b for safety
                fseek(ifp, SR2SubIFDOffset + base, SEEK_SET);
                fread(buf_SR2, SR2SubIFDLength, 1, ifp);
                sony_decrypt(buf_SR2, SR2SubIFDLength / 4, 1, SR2SubIFDKey);
                parseSonySR2 ((uchar *)buf_SR2, SR2SubIFDOffset, SR2SubIFDLength, AdobeDNG);

                free(buf_SR2);
              }

            } /* SR2 processed */
            break;
          }
        }

      } else {
        fread(mbuf + 6, 1, 2, ifp);
        if (!strcmp(mbuf, "PENTAX ") || !strcmp(mbuf, "SAMSUNG")) {
          makernote_found = 1;
          fseek(ifp, start_pos, SEEK_SET);
          parse_makernote_0xc634(base, 0, CameraDNG);
        }
      }
      fseek(ifp, start_pos, SEEK_SET);
      order = m_sorder;
    }
// IB end
#endif
      if (dng_version) {
        break;
      }
      parse_minolta(j = get4() + base);
      fseek(ifp, j, SEEK_SET);
      parse_tiff_ifd(base);
      break;
    case 50752:
      read_shorts(cr2_slice, 3);
      break;
    case 50829: /* ActiveArea */
      top_margin = getint(type);
      left_margin = getint(type);
      height = getint(type) - top_margin;
      width = getint(type) - left_margin;
      break;
    case 50830: /* MaskedAreas */
      for (i = 0; i < len && i < 32; i++)
        ((int *)mask)[i] = getint(type);
      black = 0;
      break;
#ifdef LIBRAW_LIBRARY_BUILD
    case 50970: /* PreviewColorSpace */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_PREVIEWCS;
      tiff_ifd[ifd].dng_levels.preview_colorspace = getint(type);
      break;
#endif
    case 51009: /* OpcodeList2 */
#ifdef LIBRAW_LIBRARY_BUILD
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_OPCODE2;
      tiff_ifd[ifd].opcode2_offset =
#endif
          meta_offset = ftell(ifp);
      break;
    case 64772: /* Kodak P-series */
      if (len < 13)
        break;
      fseek(ifp, 16, SEEK_CUR);
      data_offset = get4();
      fseek(ifp, 28, SEEK_CUR);
      data_offset += get4();
      load_raw = &CLASS packed_load_raw;
      break;
    case 65026:
      if (type == 2)
        fgets(model2, 64, ifp);
    }
    fseek(ifp, save, SEEK_SET);
  }
  if (sony_length && sony_length < 10240000 && (buf = (unsigned *)malloc(sony_length)))
  {
    fseek(ifp, sony_offset, SEEK_SET);
    fread(buf, sony_length, 1, ifp);
    sony_decrypt(buf, sony_length / 4, 1, sony_key);
#ifndef LIBRAW_LIBRARY_BUILD
    sfp = ifp;
    if ((ifp = tmpfile()))
    {
      fwrite(buf, sony_length, 1, ifp);
      fseek(ifp, 0, SEEK_SET);
      parse_tiff_ifd(-sony_offset);
      fclose(ifp);
    }
    ifp = sfp;
#else
    parseSonySR2 ((uchar *)buf, sony_offset, sony_length, nonDNG);
#endif
    free(buf);
  }
  for (i = 0; i < colors; i++)
    FORCC cc[i][c] *= ab[i];
  if (use_cm)
  {
    FORCC for (i = 0; i < 3; i++) for (cam_xyz[c][i] = j = 0; j < colors; j++) cam_xyz[c][i] +=
        cc[c][j] * cm[j][i] * xyz[i];
    cam_xyz_coeff(cmatrix, cam_xyz);
  }
  if (asn[0])
  {
    cam_mul[3] = 0;
    FORCC
     if(fabs(asn[c])>0.0001)
     	cam_mul[c] = 1 / asn[c];
  }
  if (!use_cm)
    FORCC if(fabs(cc[c][c])>0.0001) pre_mul[c] /= cc[c][c];
  return 0;
}