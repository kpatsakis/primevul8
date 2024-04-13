int LibRaw::parse_tiff_ifd(int base)
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

  ushort *rafdata;

  if (tiff_nifds >= sizeof tiff_ifd / sizeof tiff_ifd[0])
    return 1;
  ifd = tiff_nifds++;
  for (j = 0; j < 4; j++)
    for (i = 0; i < 4; i++)
      cc[j][i] = i == j;
  entries = get2();
  if (entries > 512)
    return 1;

  INT64 fsize = ifp->size();

  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
    INT64 savepos = ftell(ifp);
    if (len > 8 && savepos + len > 2 * fsize)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    if (callbacks.exif_cb)
    {
      callbacks.exif_cb(callbacks.exifparser_data,
                        tag | (is_pana_raw ? 0x30000 : ((ifd + 1) << 20)), type,
                        len, order, ifp, base);
      fseek(ifp, savepos, SEEK_SET);
    }

    if (!is_pana_raw)
    { /* processing of EXIF tags that collide w/ PanasonicRaw tags */
      switch (tag)
      {
      case 0x0001:
        if (len == 4)
          is_pana_raw = get4();
        break;
      case 0x000b: /* 11, Std. EXIF Software tag */
        fgets(software, 64, ifp);
        if (!strncmp(software, "Adobe", 5) || !strncmp(software, "dcraw", 5) ||
            !strncmp(software, "UFRaw", 5) || !strncmp(software, "Bibble", 6) ||
            !strcmp(software, "Digital Photo Professional"))
          is_raw = 0;
        break;
      case 0x001c: /*  28, safeguard, probably not needed */
      case 0x001d: /*  29, safeguard, probably not needed */
      case 0x001e: /*  30, safeguard, probably not needed */
        cblack[tag - 0x001c] = get2();
        cblack[3] = cblack[1];
        break;

      case 0x0111: /* 273, StripOffset */
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
      case 0x0201: /* 513, JpegIFOffset */
      case 0xf007: // 61447
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
      }
    }
    else
    { /* processing Panasonic-specific "PanasonicRaw" tags */
      switch (tag)
      {
      case 0x0004: /*   4, SensorTopBorder */
        imgdata.sizes.raw_inset_crop.ctop = get2();
        break;
      case 0x000a: /*  10, BitsPerSample */
        pana_bpp = get2();
        break;
      case 0x000b: /*  11, Compression */
        imPana.Compression = get2();
        break;
      case 0x000e: /*  14, LinearityLimitRed */
      case 0x000f: /*  15, LinearityLimitGreen */
      case 0x0010: /*  16, LinearityLimitBlue */
        imgdata.color.linear_max[tag - 14] = get2();
        if (tag == 0x000f) // 15, LinearityLimitGreen
          imgdata.color.linear_max[3] = imgdata.color.linear_max[1];
        break;
      case 0x0013: /*  19, WBInfo */
        if ((i = get2()) > 0x100)
          break;
        for (c = 0; c < i; c++)
        {
          if ((j = get2()) < 0x100)
          {
            imgdata.color.WB_Coeffs[j][0] = get2();
            imgdata.color.WB_Coeffs[j][2] = get2();
            imgdata.color.WB_Coeffs[j][1] = imgdata.color.WB_Coeffs[j][3] =
                0x100;
          }
          else // light source out of EXIF numbers range
            get4();
        }
        break;
      case 0x0018: /* 24, HighISOMultiplierRed */
      case 0x0019: /* 25, HighISOMultiplierGreen */
      case 0x001a: /* 26, HighISOMultiplierBlue */
        imPana.HighISOMultiplier[tag - 0x0018] = get2();
        break;
      case 0x001c: /*  28, BlackLevelRed */
      case 0x001d: /*  29, BlackLevelGreen */
      case 0x001e: /*  30, BlackLevelBlue */
        pana_black[tag - 0x001c] = get2();
        break;
      case 0x002d: /*  45, RawFormat */
                   /* pana_encoding: tag 0x002d (45dec)
                        not used - DMC-LX1/FZ30/FZ50/L1/LX1/LX2
                        2 - RAW DMC-FZ8/FZ18
                        3 - RAW DMC-L10
                        4 - RW2 for most other models, including G9 in "pixel shift off"
                      mode and YUNEEC CGO4            (must add 15 to black levels for RawFormat == 4)
                        5 - RW2 DC-GH5s; G9 in "pixel shift on" mode
                        6 - RW2 DC-S1, DC-S1R in "pixel shift off" mode
                        7 - RW2 DC-S1R (probably DC-S1 too) in "pixel shift on" mode
                   */
        pana_encoding = get2();
        break;
      case 0x002f: /*  47, CropTop */
        imgdata.sizes.raw_inset_crop.ctop = get2();
        break;
      case 0x0030: /*  48, CropLeft */
        imgdata.sizes.raw_inset_crop.cleft = get2();
        break;
      case 0x0031: /*  49, CropBottom */
        imgdata.sizes.raw_inset_crop.cheight =
            get2() - imgdata.sizes.raw_inset_crop.ctop;
        break;
      case 0x0032: /*  50, CropRight */
        imgdata.sizes.raw_inset_crop.cwidth =
            get2() - imgdata.sizes.raw_inset_crop.cleft;
        break;
      case 0x0037: /*  55, ISO if  ISO in 0x8827 & ISO in 0x0017 == 65535 */
        if (iso_speed == 65535)
          iso_speed = get4();
        break;
      case 0x011c: /* 284, Gamma */
      {
        int n = get2();
        if (n >= 1024)
          imPana.gamma = (float)n / 1024.0f;
        else if (n >= 256)
          imPana.gamma = (float)n / 256.0f;
        else
          imPana.gamma = (float)n / 100.0f;
      }
      break;
      case 0x0120: /* 288, CameraIFD, contains tags 0x1xxx, 0x2xxx, 0x3xxx */
      {
        unsigned sorder = order;
        unsigned long sbase = base;
        base = ftell(ifp);
        order = get2();
        fseek(ifp, 2, SEEK_CUR);
        fseek(ifp, get4() - 8, SEEK_CUR);
        parse_tiff_ifd(base);
        base = sbase;
        order = sorder;
      }
      break;
      case 0x0121: /* 289, Multishot, 0 is Off, 65536 is Pixel Shift */
        imPana.Multishot = get4();
        break;
      case 0x1203: /* 4611, FocalLengthIn35mmFormat, contained in 0x0120
                      CameraIFD */
        if (imgdata.lens.FocalLengthIn35mmFormat < 0.65f)
          imgdata.lens.FocalLengthIn35mmFormat = get2();
        break;
      case 0x2009: /* 8201, contained in 0x0120 CameraIFD */
        if ((pana_encoding == 4) || (pana_encoding == 5))
        {
          i = MIN(8, len);
          int permut[8] = {3, 2, 1, 0, 3 + 4, 2 + 4, 1 + 4, 0 + 4};
          imPana.BlackLevelDim = len;
          for (j = 0; j < i; j++)
          {
            imPana.BlackLevel[permut[j]] =
                (float)(get2()) / (float)(powf(2.f, 14.f - pana_bpp));
          }
        }
        break;
      case 0x3420: /* 13344, WB_RedLevelAuto, contained in 0x0120 CameraIFD */
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][0] = get2();
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][1] =
            imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][3] = 1024.0f;
        break;
      case 0x3421: /* 13345, WB_BlueLevelAuto, contained in 0x0120 CameraIFD */
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][2] = get2();
        break;
      case 0x0002: /*   2, ImageWidth */
        tiff_ifd[ifd].t_width = getint(type);
        break;
      case 0x0003: /*   3, ImageHeight */
        tiff_ifd[ifd].t_height = getint(type);
        break;
      case 0x0005: /*   5, SensorLeftBorder */
        width = get2();
        imgdata.sizes.raw_inset_crop.cleft = width;
        break;
      case 0x0006: /*   6, SensorBottomBorder */
        height = get2();
        imgdata.sizes.raw_inset_crop.cheight =
            height - imgdata.sizes.raw_inset_crop.ctop;
        break;
      case 0x0007: /*   7, SensorRightBorder */
        i = get2();
        width += i;
        imgdata.sizes.raw_inset_crop.cwidth =
            i - imgdata.sizes.raw_inset_crop.cleft;
        break;
      case 0x0009: /*   9, CFAPattern */
        if ((i = get2()))
          filters = i;
        break;
      case 0x0011: /*  17, RedBalance */
      case 0x0012: /*  18, BlueBalance */
        if (type == 3 && len == 1)
          cam_mul[(tag - 0x0011) * 2] = get2() / 256.0;
        break;
      case 0x0017: /*  23, ISO */
        if (type == 3)
          iso_speed = get2();
        break;
      case 0x0024: /*  36, WBRedLevel */
      case 0x0025: /*  37, WBGreenLevel */
      case 0x0026: /*  38, WBBlueLevel */
        cam_mul[tag - 0x0024] = get2();
        break;
      case 0x0027: /*  39, WBInfo2 */
        if ((i = get2()) > 0x100)
          break;
        for (c = 0; c < i; c++)
        {
          if ((j = get2()) < 0x100)
          {
            imgdata.color.WB_Coeffs[j][0] = get2();
            imgdata.color.WB_Coeffs[j][1] = imgdata.color.WB_Coeffs[j][3] =
                get2();
            imgdata.color.WB_Coeffs[j][2] = get2();
          }
          else
            fseek(ifp, 6, SEEK_CUR);
        }
        break;
        if (len < 50 || cam_mul[0] > 0.001f)
          break;
        fseek(ifp, 12, SEEK_CUR);
        FORC3 cam_mul[c] = get2();
        break;
      case 0x002e: /*  46, JpgFromRaw */
        if (type != 7 || fgetc(ifp) != 0xff || fgetc(ifp) != 0xd8)
          break;
        thumb_offset = ftell(ifp) - 2;
        thumb_length = len;
        break;

      case 0x0118: /* 280, Panasonic RW2 offset */
        if (type != 4)
          break;
        load_raw = &LibRaw::panasonic_load_raw;
        load_flags = 0x2008;
      case 0x0111: /* 273, StripOffset */
        if (len > 1 && len < 16384)
        {
          off_t sav = ftell(ifp);
          tiff_ifd[ifd].strip_offsets = (int *)calloc(len, sizeof(int));
          tiff_ifd[ifd].strip_offsets_count = len;
          for (int i = 0; i < len; i++)
            tiff_ifd[ifd].strip_offsets[i] = get4() + base;
          fseek(ifp, sav, SEEK_SET); // restore position
        }
        /* fallthrough */
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
      }

    } /* processing of Panasonic-specific tags finished */

    switch (tag)
    {            /* processing of general EXIF tags */
    case 0xf000: /* 61440, Fuji HS10 table */
      fseek(ifp, get4() + base, SEEK_SET);
      parse_tiff_ifd(base);
      break;
    case 0x00fe: /* NewSubfileType */
        tiff_ifd[ifd].newsubfiletype = getreal(type);
        break;
    case 0x0100: /* 256, ImageWidth */
    case 0xf001: /* 61441, Fuji RAF IFD 0xf001 RawImageFullWidth */
      tiff_ifd[ifd].t_width = getint(type);
      break;
    case 0x0101: /* 257, ImageHeight */
    case 0xf002: /* 61442, Fuji RAF IFD 0xf002 RawImageFullHeight */
      tiff_ifd[ifd].t_height = getint(type);
      break;
    case 0x0102: /* 258, BitsPerSample */
    case 0xf003: /* 61443, Fuji RAF IFD 0xf003 */
      tiff_ifd[ifd].samples = len & 7;
      tiff_ifd[ifd].bps = getint(type);
      if (tiff_bps < tiff_ifd[ifd].bps)
        tiff_bps = tiff_ifd[ifd].bps;
      break;
    case 0xf006: /* 61446, Fuji RAF IFD 0xf006 */
      raw_height = 0;
      if (tiff_ifd[ifd].bps > 12)
        break;
      load_raw = &LibRaw::packed_load_raw;
      load_flags = get4() ? 24 : 80;
      break;
    case 0x0103: /* 259, Compression */
                 /*
                  262	  = Kodak 262
                  32767  = Sony ARW Compressed
                  32769  = Packed RAW
                  32770  = Samsung SRW Compressed
                  32772  = Samsung SRW Compressed 2
                  32867  = Kodak KDC Compressed
                  34713  = Nikon NEF Compressed
                  65000  = Kodak DCR Compressed
                  65535  = Pentax PEF Compressed
                 */
      tiff_ifd[ifd].comp = getint(type);
      break;
    case 0x0106: /* 262, PhotometricInterpretation */
      tiff_ifd[ifd].phint = get2();
      break;
    case 0x010e: /* 270, ImageDescription */
      fread(desc, 512, 1, ifp);
      break;
    case 0x010f: /* 271, Make */
      fgets(make, 64, ifp);
      break;
    case 0x0110: /* 272, Model */
      if (!strncmp(make, "Hasselblad", 10) &&
          model[0] &&
          (imHassy.format != LIBRAW_HF_Imacon))
        break;
      fgets(model, 64, ifp);
      break;
    case 0x0116: // 278
      tiff_ifd[ifd].rows_per_strip = getint(type);
      break;
    case 0x0112: /* 274, Orientation */
      tiff_ifd[ifd].t_flip = "50132467"[get2() & 7] - '0';
      break;
    case 0x0115: /* 277, SamplesPerPixel */
      tiff_ifd[ifd].samples = getint(type) & 7;
      break;
    case 0x0117: /* 279, StripByteCounts */
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
    case 0x0202: // 514
    case 0xf008: // 61448
      tiff_ifd[ifd].bytes = get4();
      break;
    case 0xf00e: // 61454, FujiFilm "As Shot"
      FORC3 cam_mul[(4 - c) % 3] = getint(type);
      break;
    case 0x0131: /* 305, Software */
      fgets(software, 64, ifp);
      if (!strncmp(software, "Adobe", 5) || !strncmp(software, "dcraw", 5) ||
          !strncmp(software, "UFRaw", 5) || !strncmp(software, "Bibble", 6) ||
          !strcmp(software, "Digital Photo Professional"))
        is_raw = 0;
      break;
    case 0x0132: /* 306, DateTime */
      get_timestamp(0);
      break;
    case 0x013b: /* 315, Artist */
      fread(artist, 64, 1, ifp);
      break;
    case 0x013d: // 317
      tiff_ifd[ifd].predictor = getint(type);
      break;
    case 0x0142: /* 322, TileWidth */
      tiff_ifd[ifd].t_tile_width = getint(type);
      break;
    case 0x0143: /* 323, TileLength */
      tiff_ifd[ifd].t_tile_length = getint(type);
      break;
    case 0x0144: /* 324, TileOffsets */
      tiff_ifd[ifd].offset = len > 1 ? ftell(ifp) : get4();
      if (len == 1)
        tiff_ifd[ifd].t_tile_width = tiff_ifd[ifd].t_tile_length = 0;
      if (len == 4)
      {
        load_raw = &LibRaw::sinar_4shot_load_raw;
        is_raw = 5;
      }
      break;
    case 0x0145: // 325
      tiff_ifd[ifd].bytes = len > 1 ? ftell(ifp) : get4();
      break;
    case 0x014a: /* 330, SubIFDs */
      if (!strcmp(model, "DSLR-A100") && tiff_ifd[ifd].t_width == 3872)
      {
        load_raw = &LibRaw::sony_arw_load_raw;
        data_offset = get4() + base;
        ifd++;
        if (ifd >= sizeof tiff_ifd / sizeof tiff_ifd[0])
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
        break;
      }
      if (!strncmp(make, "Hasselblad", 10) &&
          libraw_internal_data.unpacker_data.hasselblad_parser_flag)
      {
        fseek(ifp, ftell(ifp) + 4, SEEK_SET);
        fseek(ifp, get4() + base, SEEK_SET);
        parse_tiff_ifd(base);
        break;
      }
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
    case 0x0153: // 339
      tiff_ifd[ifd].sample_format = getint(type);
      break;
    case 0x0190: // 400
      strcpy(make, "Sarnoff");
      maximum = 0xfff;
      break;
    case 0x02bc: // 700
      if ((type == 1 || type == 2 || type == 6 || type == 7) && len > 1 &&
          len < 5100000)
      {
        xmpdata = (char *)malloc(xmplen = len + 1);
        fread(xmpdata, len, 1, ifp);
        xmpdata[len] = 0;
      }
      break;
    case 0x7000:
      imSony.SonyRawFileType = get2();
      break;
    case 0x7010: // 28688
      FORC4 sony_curve[c + 1] = get2() >> 2 & 0xfff;
      for (i = 0; i < 5; i++)
        for (j = sony_curve[i] + 1; j <= sony_curve[i + 1]; j++)
          curve[j] = curve[j - 1] + (1 << i);
      break;
    case 0x7200: // 29184, Sony SR2Private
      sony_offset = get4();
      break;
    case 0x7201: // 29185, Sony SR2Private
      sony_length = get4();
      break;
    case 0x7221: // 29217, Sony SR2Private
      sony_key = get4();
      break;
    case 0x7250: // 29264, Sony SR2Private
      parse_minolta(ftell(ifp));
      raw_width = 0;
      break;
    case 0x7303: // 29443, Sony SR2SubIFD
      FORC4 cam_mul[c ^ (c < 2)] = get2();
      break;
    case 0x7313: // 29459, Sony SR2SubIFD
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
      break;
    case 0x7310: // 29456, Sony SR2SubIFD
      FORC4 cblack[c ^ c >> 1] = get2();
      i = cblack[3];
      FORC3 if (i > cblack[c]) i = cblack[c];
      FORC4 cblack[c] -= i;
      black = i;
      break;
    case 0x827d: /* 33405, Model2 */
                 /*
                  for Kodak ProBack 645 PB645x-yyyy 'x' is:
                  'M' for Mamiya 645
                  'C' for Contax 645
                  'H' for Hasselblad H-series
                 */
      fgets(model2, 64, ifp);
      break;
    case 0x828d: /* 33421, CFARepeatPatternDim */
      if (get2() == 6 && get2() == 6)
        tiff_ifd[ifd].t_filters = filters = 9;
      break;
    case 0x828e: /* 33422, CFAPattern */
      if (filters == 9)
      {
        FORC(36)((char *)xtrans)[c] = fgetc(ifp) & 3;
        break;
      }
    case 0xfd09: /* 64777, Kodak P-series */
      if (len == 36)
      {
        tiff_ifd[ifd].t_filters = filters = 9;
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
          if (cfa_pat[i] > 31)
            continue; // Skip wrong data
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
    case 0x8290: // 33424
    case 0xfe00: // 65024
      fseek(ifp, get4() + base, SEEK_SET);
      parse_kodak_ifd(base);
      break;
    case 0x829a: /* 33434, ExposureTime */
      tiff_ifd[ifd].t_shutter = shutter = getreal(type);
      break;
    case 0x829d: /* 33437, FNumber */
      aperture = getreal(type);
      break;
    case 0x9400:
      imgdata.makernotes.common.exifAmbientTemperature = getreal(type);
      if ((imgdata.makernotes.common.CameraTemperature > -273.15f) &&
          ((OlyID == 0x4434353933ULL) || (OlyID == 0x4434363033ULL))) // TG-5/6
        imgdata.makernotes.common.CameraTemperature += imgdata.makernotes.common.exifAmbientTemperature;
      break;
    case 0x9401:
      imgdata.makernotes.common.exifHumidity = getreal(type);
      break;
    case 0x9402:
      imgdata.makernotes.common.exifPressure = getreal(type);
      break;
    case 0x9403:
      imgdata.makernotes.common.exifWaterDepth = getreal(type);
      break;
    case 0x9404:
      imgdata.makernotes.common.exifAcceleration = getreal(type);
      break;
    case 0x9405:
      imgdata.makernotes.common.exifCameraElevationAngle = getreal(type);
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
    case 0xa420: /* 42016, ImageUniqueID */
      stmread(imgdata.color.ImageUniqueID, len, ifp);
      break;
    case 0xc65d: /* 50781, RawDataUniqueID */
      imgdata.color.RawDataUniqueID[16] = 0;
      fread(imgdata.color.RawDataUniqueID, 1, 16, ifp);
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
    case 0x8602: /* 34306, Leaf white balance */
      FORC4
      {
        int q = get2();
        if (q)
          cam_mul[c ^ 1] = 4096.0 / q;
      }
      break;
    case 0x8603: /* 34307, Leaf CatchLight color matrix */
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
    case 0x8606: /* 34310, Leaf metadata */
      parse_mos(ftell(ifp));
    case 0x85ff: // 34303
      strcpy(make, "Leaf");
      break;
    case 0x8769: /* 34665, EXIF tag */
      fseek(ifp, get4() + base, SEEK_SET);
      parse_exif(base);
      break;
    case 0x8825: /* 34853, GPSInfo tag */
    {
      unsigned pos;
      fseek(ifp, pos = (get4() + base), SEEK_SET);
      parse_gps(base);
      fseek(ifp, pos, SEEK_SET);
      parse_gps_libraw(base);
    }
    break;
    case 0x8773: /* 34675, InterColorProfile */
    case 0xc68f: /* 50831, AsShotICCProfile */
      profile_offset = ftell(ifp);
      profile_length = len;
      break;
    case 0x9102: /* 37122, CompressedBitsPerPixel */
      kodak_cbpp = get4();
      break;
    case 0x920a: /* 37386, FocalLength */
      focal_len = getreal(type);
      break;
    case 0x9211: /* 37393, ImageNumber */
      shot_order = getint(type);
      break;
    case 0x9215: /* 37397, ExposureIndex */
      imgdata.makernotes.common.exifExposureIndex = getreal(type);
      break;
    case 0x9218: /* 37400, old Kodak KDC tag */
      for (raw_color = i = 0; i < 3; i++)
      {
        getreal(type);
        FORC3 rgb_cam[i][c] = getreal(type);
      }
      break;
    case 0xa010: // 40976
      strip_offset = get4();
      switch (tiff_ifd[ifd].comp)
      {
      case 0x8002: // 32770
        load_raw = &LibRaw::samsung_load_raw;
        break;
      case 0x8004: // 32772
        load_raw = &LibRaw::samsung2_load_raw;
        break;
      case 0x8005: // 32773
        load_raw = &LibRaw::samsung3_load_raw;
        break;
      }
      break;
    case 0xb4c3: /* 46275, Imacon tags */
      imHassy.format = LIBRAW_HF_Imacon;
      strcpy(make, "Imacon");
      data_offset = ftell(ifp);
      ima_len = len;
      break;
    case 0xb4c7: // 46279
      if (!ima_len)
        break;
      fseek(ifp, 38, SEEK_CUR);
    case 0xb4c2: // 46274
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
      c = height * width / 1000000;
      if (c == 32) c--;
      sprintf(model, "Ixpress %d-Mp", c);
      load_raw = &LibRaw::imacon_full_load_raw;
      if (filters)
      {
        if (left_margin & 1)
          filters = 0x61616161;
        load_raw = &LibRaw::unpacked_load_raw;
      }
      maximum = 0xffff;
      break;
    case 0xc516: /* 50454, Sinar tag */
    case 0xc517: // 50455
      if (len < 1 || len > 2560000 || !(cbuf = (char *)malloc(len)))
        break;
      if (fread(cbuf, 1, len, ifp) != len)
        throw LIBRAW_EXCEPTION_IO_CORRUPT; // cbuf to be free'ed in recycle
      cbuf[len - 1] = 0;
      for (cp = cbuf - 1; cp && cp < cbuf + len; cp = strchr(cp, '\n'))
        if (!strncmp(++cp, "Neutral ", 8))
          sscanf(cp + 8, "%f %f %f", cam_mul, cam_mul + 1, cam_mul + 2);
      free(cbuf);
      break;
    case 0xc51a: // 50458
      if (!make[0])
        strcpy(make, "Hasselblad");
      break;
    case 0xc51b: /* 50459, Hasselblad tag */
      if (!libraw_internal_data.unpacker_data.hasselblad_parser_flag)
      {
        libraw_internal_data.unpacker_data.hasselblad_parser_flag = 1;
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
      }
    case 0xc612: /* 50706, DNGVersion */
      FORC4 dng_version = (dng_version << 8) + fgetc(ifp);
      if (!make[0])
        strcpy(make, "DNG");
      is_raw = 1;
      break;
    case 0xc614: /* 50708, UniqueCameraModel */
      stmread(imgdata.color.UniqueCameraModel, len, ifp);
      if (model[0])
        break;
      strncpy(make, imgdata.color.UniqueCameraModel,
              MIN(len, sizeof(imgdata.color.UniqueCameraModel)));
      if ((cp = strchr(make, ' ')))
      {
        strcpy(model, cp + 1);
        *cp = 0;
      }
      break;
    case 0xc616: /* 50710, CFAPlaneColor */
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
      tiff_ifd[ifd].t_filters = filters;
      break;
    case 0xc617: /* 50711, CFALayout */
      if (get2() == 2)
       tiff_ifd[ifd].t_fuji_width = fuji_width = 1;
      break;
    case 0x0123: // 291
    case 0xc618: /* 50712, LinearizationTable */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_LINTABLE;
      tiff_ifd[ifd].lineartable_offset = ftell(ifp);
      tiff_ifd[ifd].lineartable_len = len;
      linear_table(len);
      break;
    case 0xc619: /* 50713, BlackLevelRepeatDim */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
      tiff_ifd[ifd].dng_levels.dng_fcblack[4] =
          tiff_ifd[ifd].dng_levels.dng_cblack[4] = cblack[4] = get2();
      tiff_ifd[ifd].dng_levels.dng_fcblack[5] =
          tiff_ifd[ifd].dng_levels.dng_cblack[5] = cblack[5] = get2();
      if (cblack[4] * cblack[5] > (LIBRAW_CBLACK_SIZE - 7)) // Use last cblack item as DNG black level count
        tiff_ifd[ifd].dng_levels.dng_fcblack[4] =
            tiff_ifd[ifd].dng_levels.dng_fcblack[5] =
                tiff_ifd[ifd].dng_levels.dng_cblack[4] =
                    tiff_ifd[ifd].dng_levels.dng_cblack[5] = cblack[4] =
                        cblack[5] = 1;
      break;

    case 0xf00c:
      if (!is_4K_RAFdata)
      {
        unsigned fwb[4];
        FORC4 fwb[c] = get4();
        if (fwb[3] < 0x100)
        {
          imgdata.color.WB_Coeffs[fwb[3]][0] = fwb[1];
          imgdata.color.WB_Coeffs[fwb[3]][1] =
              imgdata.color.WB_Coeffs[fwb[3]][3] = fwb[0];
          imgdata.color.WB_Coeffs[fwb[3]][2] = fwb[2];
          if ((fwb[3] == 17) &&
              (libraw_internal_data.unpacker_data.lenRAFData > 3) &&
              (libraw_internal_data.unpacker_data.lenRAFData < 10240000))
          {
            INT64 f_save = ftell(ifp);
            rafdata = (ushort *)malloc(
                sizeof(ushort) * libraw_internal_data.unpacker_data.lenRAFData);
            fseek(ifp, libraw_internal_data.unpacker_data.posRAFData, SEEK_SET);
            fread(rafdata, sizeof(ushort),
                  libraw_internal_data.unpacker_data.lenRAFData, ifp);
            fseek(ifp, f_save, SEEK_SET);

            uchar *PrivateMknBuf = (uchar *)rafdata;
            int PrivateMknLength = libraw_internal_data.unpacker_data.lenRAFData
                                   << 1;
            for (int pos = 0; pos < PrivateMknLength - 16; pos++)
            {
              if (!memcmp(PrivateMknBuf + pos, "TSNERDTS", 8))
              {
                imFuji.isTSNERDTS = 1;
                break;
              }
            }

            /* 0xc000 tag version, second ushort; valid if the first ushort is 0
             */
            if (!rafdata[0])
              imFuji.RAFDataVersion = rafdata[1];
            int fj;
            for (int fi = 0;
                 fi < (libraw_internal_data.unpacker_data.lenRAFData - 3); fi++)
            { // find Tungsten WB
              if ((fwb[0] == rafdata[fi]) && (fwb[1] == rafdata[fi + 1]) &&
                  (fwb[2] == rafdata[fi + 2]))
              {
                if (rafdata[fi - 15] !=
                    fwb[0]) // 15 is offset of Tungsten WB from the first
                            // preset, Fine Weather WB
                  continue;
                for (int wb_ind = 0, ofst = fi - 15; wb_ind < nFuji_wb_list1;
                     wb_ind++, ofst += 3)
                {
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][1] =
                      imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][3] =
                          rafdata[ofst];
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][0] =
                      rafdata[ofst + 1];
                  imgdata.color.WB_Coeffs[Fuji_wb_list1[wb_ind]][2] =
                      rafdata[ofst + 2];
                }

                if (imFuji.RAFDataVersion == 0x260) { // X-Pro3
                  for (int iCCT = 0, ofst = fi+24; iCCT < 31; iCCT++, ofst += 3)  {
                    imgdata.color.WBCT_Coeffs[iCCT][0] = FujiCCT_K[iCCT];
                    imgdata.color.WBCT_Coeffs[iCCT][1] =
                        rafdata[ofst+1];
                    imgdata.color.WBCT_Coeffs[iCCT][2] =
                        imgdata.color.WBCT_Coeffs[iCCT][4] =
                            rafdata[ofst];
                    imgdata.color.WBCT_Coeffs[iCCT][3] =
                        rafdata[ofst+2];
                  }
                  free(rafdata);
                  break;
                }
                fi += 96;
                for (fj = fi; fj < (fi + 15); fj += 3) {
                  if (rafdata[fj] != rafdata[fi]) {
                    for (int iCCT = 0, ofst = fj - 93; iCCT < 31; iCCT++, ofst += 3) {
                      imgdata.color.WBCT_Coeffs[iCCT][0] = FujiCCT_K[iCCT];
                      imgdata.color.WBCT_Coeffs[iCCT][1] =
                          rafdata[ofst+1];
                      imgdata.color.WBCT_Coeffs[iCCT][2] =
                          imgdata.color.WBCT_Coeffs[iCCT][4] =
                              rafdata[ofst];
                      imgdata.color.WBCT_Coeffs[iCCT][3] =
                          rafdata[ofst+2];
                    }
                    break;
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
          imgdata.color.WB_Coeffs[fwb[3]][1] =
              imgdata.color.WB_Coeffs[fwb[3]][3] = fwb[0];
          imgdata.color.WB_Coeffs[fwb[3]][2] = fwb[2];
        }
      }
      break;
    case 0xf00d:
      if (!is_4K_RAFdata)
      {
        FORC3 imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][(4 - c) % 3] =
            getint(type);
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][3] =
            imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][1];
        //        free(rafdata);
      }
      break;
    case 0xc615: /* 50709, LocalizedCameraModel */
      stmread(imgdata.color.LocalizedCameraModel, len, ifp);
      break;
    case 0xf00a: // 61450
      cblack[4] = cblack[5] = MIN(sqrt((double)len), 64);
    case 0xc61a: /* 50714, BlackLevel */
      if (tiff_ifd[ifd].samples > 1 &&
          tiff_ifd[ifd].samples == len) // LinearDNG, per-channel black
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
        for (i = 0; i < 4 && i < len; i++)
        {
          tiff_ifd[ifd].dng_levels.dng_fcblack[i] = getreal(type);
          tiff_ifd[ifd].dng_levels.dng_cblack[i] = cblack[i] =
              tiff_ifd[ifd].dng_levels.dng_fcblack[i] + 0.5;
        }

        tiff_ifd[ifd].dng_levels.dng_fblack =
            tiff_ifd[ifd].dng_levels.dng_black = black = 0;
      }
      else if( tiff_ifd[ifd].samples > 1  // Linear DNG w repeat dim
          && (tiff_ifd[ifd].samples * cblack[4] * cblack[5] == len))
      {
          tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
          tiff_ifd[ifd].dng_levels.dng_cblack[LIBRAW_CBLACK_SIZE-1] = cblack[LIBRAW_CBLACK_SIZE-1] = len;
          for (i = 0; i < len && i < LIBRAW_CBLACK_SIZE-7; i++)
          {
              tiff_ifd[ifd].dng_levels.dng_fcblack[i+6] = getreal(type);
              tiff_ifd[ifd].dng_levels.dng_cblack[i+6] = cblack[i+6] =
                  tiff_ifd[ifd].dng_levels.dng_fcblack[i+6] + 0.5;
          }
      }
      else if ((cblack[4] * cblack[5] < 2) && len == 1)
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
        tiff_ifd[ifd].dng_levels.dng_fblack = getreal(type);
        black = tiff_ifd[ifd].dng_levels.dng_black =
            tiff_ifd[ifd].dng_levels.dng_fblack;
      }
      else if (cblack[4] * cblack[5] <= len)
      {
        FORC(cblack[4] * cblack[5])
        {
          tiff_ifd[ifd].dng_levels.dng_fcblack[6 + c] = getreal(type);
          cblack[6 + c] = tiff_ifd[ifd].dng_levels.dng_fcblack[6 + c];
        }
        black = 0;
        FORC4
        cblack[c] = 0;

        if (tag == 0xc61a)
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
      }
      break;
    case 0xc61b: /* 50715, BlackLevelDeltaH */
    case 0xc61c: /* 50716, BlackLevelDeltaV */
      for (num = i = 0; i < len && i < 65536; i++)
        num += getreal(type);
      if (len > 0)
      {
        black += num / len + 0.5;
        tiff_ifd[ifd].dng_levels.dng_fblack += num / float(len);
        tiff_ifd[ifd].dng_levels.dng_black += num / len + 0.5;
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BLACK;
      }
      break;
    case 0xc61d: /* 50717, WhiteLevel */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_WHITE;
      tiff_ifd[ifd].dng_levels.dng_whitelevel[0] = maximum = getint(type);
      if (tiff_ifd[ifd].samples > 1) // Linear DNG case
        for (i = 1; i < 4 && i < len; i++)
          tiff_ifd[ifd].dng_levels.dng_whitelevel[i] = getint(type);
      break;
    case 0xc61e: /* DefaultScale */
    {
      float q1 = getreal(type);
      float q2 = getreal(type);
      if (q1 > 0.00001f && q2 > 0.00001f)
      {
        pixel_aspect = q1 / q2;
        if (pixel_aspect > 0.995 && pixel_aspect < 1.005)
          pixel_aspect = 1.0;
      }
    }
    break;
    case 0xc61f: /* 50719, DefaultCropOrigin */
      if (len == 2)
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_CROPORIGIN;
        tiff_ifd[ifd].dng_levels.default_crop[0] = getreal(type);
        tiff_ifd[ifd].dng_levels.default_crop[1] = getreal(type);
        if (!strncasecmp(make, "SONY", 4))
        {
          imgdata.sizes.raw_inset_crop.cleft =
              tiff_ifd[ifd].dng_levels.default_crop[0];
          imgdata.sizes.raw_inset_crop.ctop =
              tiff_ifd[ifd].dng_levels.default_crop[1];
        }
      }
      break;

    case 0xc620: /* 50720, DefaultCropSize */
      if (len == 2)
      {
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_CROPSIZE;
        tiff_ifd[ifd].dng_levels.default_crop[2] = getreal(type);
        tiff_ifd[ifd].dng_levels.default_crop[3] = getreal(type);
        if (!strncasecmp(make, "SONY", 4))
        {
          imgdata.sizes.raw_inset_crop.cwidth =
              tiff_ifd[ifd].dng_levels.default_crop[2];
          imgdata.sizes.raw_inset_crop.cheight =
              tiff_ifd[ifd].dng_levels.default_crop[3];
        }
      }
      break;

    case 0x74c7:
      if ((len == 2) && !strncasecmp(make, "SONY", 4))
      {
        imgdata.sizes.raw_inset_crop.cleft = get4();
        imgdata.sizes.raw_inset_crop.ctop = get4();
      }
      break;

    case 0x74c8:
      if ((len == 2) && !strncasecmp(make, "SONY", 4))
      {
        imgdata.sizes.raw_inset_crop.cwidth = get4();
        imgdata.sizes.raw_inset_crop.cheight = get4();
      }
      break;

    case 0xc65a: // 50778
      tiff_ifd[ifd].dng_color[0].illuminant = get2();
      tiff_ifd[ifd].dng_color[0].parsedfields |= LIBRAW_DNGFM_ILLUMINANT;
      break;
    case 0xc65b: // 50779
      tiff_ifd[ifd].dng_color[1].illuminant = get2();
      tiff_ifd[ifd].dng_color[1].parsedfields |= LIBRAW_DNGFM_ILLUMINANT;
      break;

    case 0xc621: /* 50721, ColorMatrix1 */
    case 0xc622: /* 50722, ColorMatrix2 */
    {
      int chan = (len == 9) ? 3 : (len == 12 ? 4 : 0);
      i = tag == 0xc621 ? 0 : 1;
      if (chan) {
        tiff_ifd[ifd].dng_color[i].parsedfields |= LIBRAW_DNGFM_COLORMATRIX;
        imHassy.nIFD_CM[i] = ifd;
      }
      FORC(chan) for (j = 0; j < 3; j++)
      {
        tiff_ifd[ifd].dng_color[i].colormatrix[c][j] = cm[c][j] = getreal(type);
      }
      use_cm = 1;
    }
    break;

    case 0xc714: /* ForwardMatrix1 */
    case 0xc715: /* ForwardMatrix2 */
    {
      int chan = (len == 9) ? 3 : (len == 12 ? 4 : 0);
      i = tag == 0xc714 ? 0 : 1;
      if (chan)
        tiff_ifd[ifd].dng_color[i].parsedfields |= LIBRAW_DNGFM_FORWARDMATRIX;
      for (j = 0; j < 3; j++)
        FORC(chan)
        {
          tiff_ifd[ifd].dng_color[i].forwardmatrix[j][c] = fm[j][c] =
              getreal(type);
        }
    }
    break;

    case 0xc623: /* 50723, CameraCalibration1 */
    case 0xc624: /* 50724, CameraCalibration2 */
    {
      int chan = (len == 9) ? 3 : (len == 16 ? 4 : 0);
      j = tag == 0xc623 ? 0 : 1;
      if (chan)
        tiff_ifd[ifd].dng_color[j].parsedfields |= LIBRAW_DNGFM_CALIBRATION;
      for (i = 0; i < chan; i++)
        FORC(chan)
        {
          tiff_ifd[ifd].dng_color[j].calibration[i][c] = cc[i][c] =
              getreal(type);
        }
    }
    break;
    case 0xc627: /* 50727, AnalogBalance */
      if (len >= 3)
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_ANALOGBALANCE;
      for (c = 0; c < len && c < 4; c++)
      {
        tiff_ifd[ifd].dng_levels.analogbalance[c] = ab[c] = getreal(type);
      }
      break;
    case 0xc628: /* 50728, AsShotNeutral */
      if (len >= 3)
        tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_ASSHOTNEUTRAL;
      for (c = 0; c < len && c < 4; c++)
        tiff_ifd[ifd].dng_levels.asshotneutral[c] = asn[c] = getreal(type);
      break;
    case 0xc629: /* 50729, AsShotWhiteXY */
      xyz[0] = getreal(type);
      xyz[1] = getreal(type);
      xyz[2] = 1 - xyz[0] - xyz[1];
      FORC3 xyz[c] /= LibRaw_constants::d65_white[c];
      break;
    case 0xc62a: /* DNG: 50730 BaselineExposure */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_BASELINEEXPOSURE;
      tiff_ifd[ifd].dng_levels.baseline_exposure = getreal(type);
      break;
    case 0xc62e: /* DNG: 50734 LinearResponseLimit */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_LINEARRESPONSELIMIT;
      tiff_ifd[ifd].dng_levels.LinearResponseLimit = getreal(type);
      break;

    case 0xc634: /* 50740 : DNG Adobe, DNG Pentax, Sony SR2, DNG Private */
      if (!(imgdata.params.raw_processing_options &
            LIBRAW_PROCESSING_SKIP_MAKERNOTES))
      {
        char mbuf[64];
        unsigned short makernote_found = 0;
        INT64 curr_pos, start_pos = ftell(ifp);
        unsigned MakN_order, m_sorder = order;
        unsigned MakN_length;
        unsigned pos_in_original_raw;
        fread(mbuf, 1, 6, ifp);

        if (!strcmp(mbuf, "Adobe"))
        {
          order = 0x4d4d; // Adobe header is always in "MM" / big endian
          curr_pos = start_pos + 6;
          while (curr_pos + 8 - start_pos <= len)
          {
            fread(mbuf, 1, 4, ifp);
            curr_pos += 8;

            if (!strncmp(mbuf, "Pano", 4))
            { // PanasonicRaw, yes, they use "Pano" as signature
              parseAdobePanoMakernote();
            }

            if (!strncmp(mbuf, "MakN", 4))
            {
              makernote_found = 1;
              MakN_length = get4();
              MakN_order = get2();
              pos_in_original_raw = get4();
              order = MakN_order;

              INT64 save_pos = ifp->tell();
              parse_makernote_0xc634(curr_pos + 6 - pos_in_original_raw, 0,
                                     AdobeDNG);

              curr_pos = save_pos + MakN_length - 6;
              fseek(ifp, curr_pos, SEEK_SET);

              fread(mbuf, 1, 4, ifp);
              curr_pos += 8;

              if (!strncmp(mbuf, "Pano ", 4))
              {
                parseAdobePanoMakernote();
              }

              if (!strncmp(mbuf, "RAF ", 4))
              { // Fujifilm Raw, AdobeRAF
                parseAdobeRAFMakernote();
              }

              if (!strncmp(mbuf, "SR2 ", 4))
              {
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
                while (entries--)
                {
                  tiff_get(base, &tag, &type, &len, &save);

                  if (tag == 0x7200)
                  {
                    SR2SubIFDOffset = get4();
                  }
                  else if (tag == 0x7201)
                  {
                    SR2SubIFDLength = get4();
                  }
                  else if (tag == 0x7221)
                  {
                    SR2SubIFDKey = get4();
                  }
                  fseek(ifp, save, SEEK_SET);
                }

                if (SR2SubIFDLength && (SR2SubIFDLength < 10240000) &&
                    (buf_SR2 = (unsigned *)malloc(SR2SubIFDLength + 1024)))
                { // 1024b for safety
                  fseek(ifp, SR2SubIFDOffset + base, SEEK_SET);
                  fread(buf_SR2, SR2SubIFDLength, 1, ifp);
                  sony_decrypt(buf_SR2, SR2SubIFDLength / 4, 1, SR2SubIFDKey);
                  parseSonySR2((uchar *)buf_SR2, SR2SubIFDOffset,
                               SR2SubIFDLength, AdobeDNG);

                  free(buf_SR2);
                }

              } /* SR2 processed */
              break;
            }
          }
        }
        else
        {
          fread(mbuf + 6, 1, 2, ifp);
          if (!strcmp(mbuf, "RICOH") && ((sget2((uchar *)mbuf + 6) == 0x4949) ||
                                         (sget2((uchar *)mbuf + 6) == 0x4d4d)))
          {
            is_PentaxRicohMakernotes = 1;
          }
          if (!strcmp(mbuf, "PENTAX ") || !strcmp(mbuf, "SAMSUNG") ||
              is_PentaxRicohMakernotes)
          {
            makernote_found = 1;
            fseek(ifp, start_pos, SEEK_SET);
            parse_makernote_0xc634(base, 0, CameraDNG);
          }
        }
        fseek(ifp, start_pos, SEEK_SET);
        order = m_sorder;
      }
      if (dng_version)
      {
        break;
      }
      parse_minolta(j = get4() + base);
      fseek(ifp, j, SEEK_SET);
      parse_tiff_ifd(base);
      break;
    case 0xc640: // 50752
      read_shorts(cr2_slice, 3);
      break;
    case 0xc68b: /* 50827, OriginalRawFileName */
      stmread(imgdata.color.OriginalRawFileName, len, ifp);
      break;
    case 0xc68d: /* 50829 ActiveArea */
        tiff_ifd[ifd].t_tm=top_margin = getint(type);
        tiff_ifd[ifd].t_lm = left_margin = getint(type);
        tiff_ifd[ifd].t_vheight = height = getint(type) - top_margin;
        tiff_ifd[ifd].t_vwidth = width = getint(type) - left_margin;
      break;
    case 0xc68e: /* 50830 MaskedAreas */
      for (i = 0; i < len && i < 32; i++)
        ((int *)mask)[i] = getint(type);
      black = 0;
      break;
    case 0xc71a: /* 50970, PreviewColorSpace */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_PREVIEWCS;
      tiff_ifd[ifd].dng_levels.preview_colorspace = getint(type);
      break;
    case 0xc741: /* 51009, OpcodeList2 */
      tiff_ifd[ifd].dng_levels.parsedfields |= LIBRAW_DNGFM_OPCODE2;
      tiff_ifd[ifd].opcode2_offset = meta_offset = ftell(ifp);
      break;
    case 0xfd04: /* 64772, Kodak P-series */
      if (len < 13)
        break;
      fseek(ifp, 16, SEEK_CUR);
      data_offset = get4();
      fseek(ifp, 28, SEEK_CUR);
      data_offset += get4();
      load_raw = &LibRaw::packed_load_raw;
      break;
    case 0xfe02: // 65026
      if (type == 2)
        fgets(model2, 64, ifp);
    }
    fseek(ifp, save, SEEK_SET);
  }
  if (sony_length && sony_length < 10240000 &&
      (buf = (unsigned *)malloc(sony_length)))
  {
    fseek(ifp, sony_offset, SEEK_SET);
    fread(buf, sony_length, 1, ifp);
    sony_decrypt(buf, sony_length / 4, 1, sony_key);
    parseSonySR2((uchar *)buf, sony_offset, sony_length, nonDNG);
    free(buf);
  }
  for (i = 0; i < colors; i++)
    FORCC cc[i][c] *= ab[i];
  if (use_cm)
  {
    FORCC for (i = 0; i < 3; i++) for (cam_xyz[c][i] = j = 0; j < colors; j++)
        cam_xyz[c][i] += cc[c][j] * cm[j][i] * xyz[i];
    cam_xyz_coeff(cmatrix, cam_xyz);
  }
  if (asn[0])
  {
    cam_mul[3] = 0;
    FORCC
    if (fabs(asn[c]) > 0.0001)
      cam_mul[c] = 1 / asn[c];
  }
  if (!use_cm)
    FORCC if (fabs(cc[c][c]) > 0.0001) pre_mul[c] /= cc[c][c];
  return 0;
}