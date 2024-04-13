void CLASS parse_makernote(int base, int uptag)
{

#ifdef LIBRAW_LIBRARY_BUILD
  if (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SKIP_MAKERNOTES)
  	return;

  if (!strncmp(make, "NIKON", 5)) {
    parseNikonMakernote (base, uptag, nonDNG);
    return;
  } else if (!strncasecmp(make, "LEICA", 5)) {
    parseLeicaMakernote (base, uptag, is_0x927c);
    return;
  }
#endif

  unsigned offset = 0, entries, tag, type, len, save, c;
  unsigned serial = 0, i, wbi = 0, wb[4] = {0, 0, 0, 0};
  short morder, sorder = order;
  char buf[10];
  unsigned SamsungKey[11];

  uchar NikonKey;
  unsigned ver97 = 0;
  uchar buf97[324];
  uchar ci, cj, ck;


#ifdef LIBRAW_LIBRARY_BUILD
  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;
  unsigned typeCanonCameraInfo = 0;

  int isSony = 0;

  uchar *table_buf;
  uchar *table_buf_0x0116;
  ushort table_buf_0x0116_len = 0;
  uchar *table_buf_0x2010;
  ushort table_buf_0x2010_len = 0;
  uchar *table_buf_0x9050;
  ushort table_buf_0x9050_len = 0;
  uchar *table_buf_0x9400;
  ushort table_buf_0x9400_len = 0;
  uchar *table_buf_0x9402;
  ushort table_buf_0x9402_len = 0;
  uchar *table_buf_0x9403;
  ushort table_buf_0x9403_len = 0;
  uchar *table_buf_0x9406;
  ushort table_buf_0x9406_len = 0;
  uchar *table_buf_0x940c;
  ushort table_buf_0x940c_len = 0;
  uchar *table_buf_0x940e;
  ushort table_buf_0x940e_len = 0;

  INT64 fsize = ifp->size();
#endif
/*
     The MakerNote might have its own TIFF header (possibly with
     its own byte-order!), or it might just be a table.
*/
  if (!strncmp(make, "Nokia", 5))
    return;

  fread(buf, 1, 10, ifp);

  if (!strncmp(buf, "KDK", 3)  || /* these aren't TIFF tables */
      !strncmp(buf, "VER", 3)  ||
      !strncmp(buf, "IIII", 4) ||
      !strncmp(buf, "MMMM", 4))
    return;

  if (!strncmp(buf, "KC", 2) || /* Konica KD-400Z, KD-510Z */
      !strncmp(buf, "MLY", 3))  /* Minolta DiMAGE G series */
  {
    order = 0x4d4d;
    while ((i = ftell(ifp)) < data_offset && i < 16384) {
      wb[0] = wb[2];
      wb[2] = wb[1];
      wb[1] = wb[3];
      wb[3] = get2();
      if (wb[1] == 256 && wb[3] == 256 && wb[0] > 256 && wb[0] < 640 && wb[2] > 256 && wb[2] < 640)
        FORC4 cam_mul[c] = wb[c];
    }
    goto quit;
  }

  if (!strcmp(buf, "Nikon")) {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42) goto quit;
    offset = get4();
    fseek(ifp, offset - 8, SEEK_CUR);

  } else if (!strcmp(buf, "OLYMPUS") ||
             !strcmp(buf, "PENTAX ")) {
    base = ftell(ifp) - 10;
    fseek(ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O') get2();

  } else if (!strncmp(buf, "SONY", 4) ||
           !strcmp(buf, "Panasonic")) {
    goto nf;

  } else if (!strncmp(buf, "FUJIFILM", 8)) {
    base = ftell(ifp) - 10;
  nf:
    order = 0x4949;
    fseek(ifp, 2, SEEK_CUR);

  } else if (!strcmp(buf, "OLYMP") ||
             !strncmp (buf,"LEICA", 5) ||
             !strcmp(buf, "Ricoh") ||
             !strcmp(buf, "EPSON")) {
    fseek(ifp, -2, SEEK_CUR);

  } else if (!strcmp(buf, "AOC") ||
             !strcmp(buf, "QVC")) {
    fseek(ifp, -4, SEEK_CUR);

  } else {
    fseek(ifp, -10, SEEK_CUR);
    if (!strncmp(make, "SAMSUNG", 7))
      base = ftell(ifp);
  }

#ifdef LIBRAW_LIBRARY_BUILD
  if (!strncasecmp(make, "SONY", 4)    ||
      !strncasecmp(make, "Konica", 6)  ||
      !strncasecmp(make, "Minolta", 7) ||
      (!strncasecmp(make, "Hasselblad", 10) &&
       (!strncasecmp(model, "Stellar", 7) ||
        !strncasecmp(model, "Lunar", 5)   ||
        !strncasecmp(model, "Lusso", 5)   ||
        !strncasecmp(model, "HV", 2)))
     )
   isSony = 1;

  if (strcasestr(make, "Kodak")       &&
      (sget2((uchar *)buf) > 1)       &&  // check number of entries
      (sget2((uchar *)buf) < 128)     &&
      (sget2((uchar *)(buf+4)) > 0)   &&  // check type
      (sget2((uchar *)(buf+4)) < 13)  &&
      (sget4((uchar *)(buf+6)) < 256)     // check count
     )
   imgdata.makernotes.kodak.MakerNoteKodak8a = 1;  // Kodak P712 / P850 / P880
#endif

  entries = get2();
  if (entries > 1000) return;

  morder = order;
  while (entries--) {
    order = morder;
    tiff_get(base, &tag, &type, &len, &save);
    tag |= uptag << 16;

#ifdef LIBRAW_LIBRARY_BUILD
    INT64 _pos = ftell(ifp);
    if (len > 8 && _pos + len > 2 * fsize)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    if (imgdata.makernotes.kodak.MakerNoteKodak8a) {
      if ((tag == 0xff00) && (type == 4) && (len == 1)) {
        INT64 _pos1 = get4();
        if ((_pos1 < fsize) && (_pos1 > 0)) {
          fseek(ifp, _pos1, SEEK_SET);
          parse_makernote(base, tag);
        }

      } else if (tag == 0xff00f90b) {
        imgdata.makernotes.kodak.clipBlack = get2();

      } else if (tag == 0xff00f90c) {
        imgdata.makernotes.kodak.clipWhite =
          imgdata.color.linear_max[0] =
          imgdata.color.linear_max[1] =
          imgdata.color.linear_max[2] =
          imgdata.color.linear_max[3] = get2();
      }
    }
    else if (!strncmp(make, "Canon", 5))
    {
      if (tag == 0x000d && len < 256000) // camera info
      {
        if (type != 4)
        {
          CanonCameraInfo = (uchar *)malloc(MAX(16, len));
          fread(CanonCameraInfo, len, 1, ifp);
        }
        else
        {
          CanonCameraInfo = (uchar *)malloc(MAX(16, len * 4));
          fread(CanonCameraInfo, len, 4, ifp);
        }
        lenCanonCameraInfo = len;
        typeCanonCameraInfo = type;
      }

      else if (tag == 0x10) // Canon ModelID
      {
        unique_id = get4();
        unique_id = setCanonBodyFeatures(unique_id);
        if (lenCanonCameraInfo)
        {
          processCanonCameraInfo(unique_id, CanonCameraInfo, lenCanonCameraInfo, typeCanonCameraInfo);
          free(CanonCameraInfo);
          CanonCameraInfo = 0;
          lenCanonCameraInfo = 0;
        }
      }

      else
        parseCanonMakernotes(tag, type, len);
    }

    else if (!strncmp(make, "FUJI", 4))
        parseFujiMakernotes(tag, type, len, nonDNG);

    else if (!strncasecmp(model, "Hasselblad X1D", 14) ||
             !strncasecmp(model, "Hasselblad H6D", 14) ||
             !strncasecmp(model, "Hasselblad A6D", 14))
    {
      if (tag == 0x0045)
      {
        imgdata.makernotes.hasselblad.BaseISO = get4();
      }
      else if (tag == 0x0046)
      {
        imgdata.makernotes.hasselblad.Gain = getreal(type);
      }
    }

    else if (!strncmp(make, "PENTAX", 6) ||
             !strncmp(make, "RICOH", 5)  ||
             !strncmp(model, "PENTAX", 6))
    {
      if (!strncmp(model, "GR", 2) ||
          !strncmp(model, "GXR", 3))
        parseRicohMakernotes (base, tag, type, len, CameraDNG);
      else
        parsePentaxMakernotes(base, tag, type, len, nonDNG);
    }

    else if (!strncmp(make, "SAMSUNG", 7))
    {
      if (!dng_version)
        parseSamsungMakernotes(base, tag, type, len, nonDNG);
      else
        parsePentaxMakernotes(base, tag, type, len, CameraDNG);
    }

    else if (isSony) {
      if ((tag == 0xb028) && (len == 1) && (type == 4)) { // DSLR-A100
        if ((c = get4())) {
          fseek (ifp, c, SEEK_SET);
          parse_makernote(base, tag);
        }
      } else {
      parseSonyMakernotes(base, tag, type, len, nonDNG,
                          table_buf_0x0116, table_buf_0x0116_len,
                          table_buf_0x2010, table_buf_0x2010_len,
                          table_buf_0x9050, table_buf_0x9050_len,
                          table_buf_0x9400, table_buf_0x9400_len,
                          table_buf_0x9402, table_buf_0x9402_len,
                          table_buf_0x9403, table_buf_0x9403_len,
                          table_buf_0x9406, table_buf_0x9406_len,
                          table_buf_0x940c, table_buf_0x940c_len,
                          table_buf_0x940e, table_buf_0x940e_len);
      }
    }
    fseek(ifp, _pos, SEEK_SET);
#endif

    if (tag == 2 && strstr(make, "NIKON") && !iso_speed)
      iso_speed = (get2(), get2());
    if (tag == 37 && strstr(make, "NIKON") && (!iso_speed || iso_speed == 65535))
    {
      iso_speed = int(100.0 * libraw_powf64l(2.0f, double((uchar)fgetc(ifp)) / 12.0 - 5.0));
    }
    if (tag == 4 && len > 26 && len < 35)
    {
      if ((i = (get4(), get2())) != 0x7fff && (!iso_speed || iso_speed == 65535))
        iso_speed = 50 * libraw_powf64l(2.0, i / 32.0 - 4);
#ifdef LIBRAW_LIBRARY_BUILD
      get4();
#else
      if (((i = (get2(), get2())) != 0x7fff) && !aperture)
        aperture = libraw_powf64l(2.0, i / 64.0);
#endif
      if (((i = get2()) != 0xffff) && !shutter)
        shutter = libraw_powf64l(2.0, (short)i / -32.0);
      wbi = (get2(), get2());
      shot_order = (get2(), get2());
    }
    if ((tag == 4 || tag == 0x114) && !strncmp(make, "KONICA", 6))
    {
      fseek(ifp, tag == 4 ? 140 : 160, SEEK_CUR);
      switch (get2())
      {
      case 72:
        flip = 0;
        break;
      case 76:
        flip = 6;
        break;
      case 82:
        flip = 5;
        break;
      }
    }
    if (tag == 7 && type == 2 && len > 20)
      fgets(model2, 64, ifp);
    if (tag == 8 && type == 4)
      shot_order = get4();
    if (tag == 9 && !strncmp(make, "Canon", 5))
      fread(artist, 64, 1, ifp);
    if (tag == 0xc && len == 4)
      FORC3 cam_mul[(c << 1 | c >> 1) & 3] = getreal(type);
    if (tag == 0xd && type == 7 && get2() == 0xaaaa)
    {
      for (c = i = 2; (ushort)c != 0xbbbb && i < len; i++)
        c = c << 8 | fgetc(ifp);
      while ((i += 4) < len - 5)
        if (get4() == 257 && (i = len) && (c = (get4(), fgetc(ifp))) < 3)
          imgdata.makernotes.canon.MakernotesFlip  = "065"[c] - '0';
    }

#ifndef LIBRAW_LIBRARY_BUILD
    if (tag == 0x10 && type == 4)
      unique_id = get4();
#endif

#ifdef LIBRAW_LIBRARY_BUILD
    INT64 _pos2 = ftell(ifp);
    if (!strncasecmp(make, "Olympus", 7)) {

      if ((tag == 0x2010) ||
          (tag == 0x2020) ||
          (tag == 0x2030) ||
          (tag == 0x2031) ||
          (tag == 0x2040) ||
          (tag == 0x2050) ||
          (tag == 0x3000)) {
        if (type == 7) { parse_makernote(base, tag); }
        else if (type == 13) { fseek(ifp, base+get4(), SEEK_SET); parse_makernote(base, tag); }

      } else if (tag == 0x0207) {
          getOlympus_CameraType2();

      } else if ((tag == 0x0404) || (tag == 0x101a)) {
          if (!imgdata.shootinginfo.BodySerial[0])
            stmread(imgdata.shootinginfo.BodySerial, len, ifp);

      } else if (tag == 0x1002) {
          imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, getreal(type) / 2);
      } else if (tag == 0x1007) {
          imgdata.other.SensorTemperature = (float)get2();
      } else if (tag == 0x1008) {
          imgdata.other.LensTemperature = (float)get2();

      } else if ((tag == 0x1011) && strcmp(software, "v757-71")) {
          for (i = 0; i < 3; i++) {
            if (!imgdata.makernotes.olympus.ColorSpace) {
              FORC3 cmatrix[i][c] = ((short)get2()) / 256.0;
            } else {
              FORC3 imgdata.color.ccm[i][c] = ((short)get2()) / 256.0;
            }
          }

      } else if (tag == 0x1012) {
          FORC4 cblack[c ^ c >> 1] = get2();
      } else if (tag == 0x1017) {
          cam_mul[0] = get2() / 256.0;
      } else if (tag == 0x1018) {
          cam_mul[2] = get2() / 256.0;

      } else if ((tag >= 0x20100000) && (tag <= 0x2010ffff)) {
          parseOlympus_Equipment ((tag & 0x0000ffff), type, len, nonDNG);

      } else if ((tag >= 0x20200000) && (tag <= 0x2020ffff)) {
          parseOlympus_CameraSettings (base, (tag & 0x0000ffff), type, len, nonDNG);

      } else if ((tag == 0x20300108) || (tag == 0x20310109)) {
          imgdata.makernotes.olympus.ColorSpace = get2();

      } else if ((tag >= 0x20400000) && (tag <= 0x2040ffff)) {
          parseOlympus_ImageProcessing ((tag & 0x0000ffff), type, len, nonDNG);

      } else if (tag == 0x20501500) {
          getOlympus_SensorTemperature(len);

      } else if ((tag >= 0x30000000) && (tag <= 0x3000ffff)) {
          parseOlympus_RawInfo ((tag & 0x0000ffff), type, len, nonDNG);
      }

    }
    fseek(ifp, _pos2, SEEK_SET);
#endif

    if (tag == 0x11 && is_raw && !strncmp(make, "NIKON", 5))
    {
      fseek(ifp, get4() + base, SEEK_SET);
      parse_tiff_ifd(base);
    }

    if (tag == 0x14 && type == 7) {
      if (len == 2560) {
        fseek(ifp, 1248, SEEK_CUR);
        goto get2_256;
      } else if (len == 1280) {
        cam_mul[0] = cam_mul[1] = cam_mul[2] = cam_mul[3] = 1.0;

      } else {
        fread(buf, 1, 10, ifp);
        if (!strncmp(buf, "NRW ", 4)) {
          fseek(ifp, strcmp(buf + 4, "0100") ? 46 : 1546, SEEK_CUR);
          cam_mul[0] = get4() << 2;
          cam_mul[1] = get4() + get4();
          cam_mul[2] = get4() << 2;
        }
      }
    }

    if (tag == 0x15 && type == 2 && is_raw)
      fread(model, 64, 1, ifp);
    if (strstr(make, "PENTAX"))
    {
      if (tag == 0x1b)
        tag = 0x1018;
      if (tag == 0x1c)
        tag = 0x1017;
    }
    if ((tag == 0x1d) && (type == 2) && (len > 0))
    {
      while ((c = fgetc(ifp)) && (len-- > 0) && (c != EOF))
        serial = serial * 10 + (isdigit(c) ? c - '0' : c % 10);
    }
    if (tag == 0x29 && type == 1)
    { // Canon PowerShot G9
      c = wbi < 18 ? "012347800000005896"[wbi] - '0' : 0;
      fseek(ifp, 8 + c * 32, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get4();
    }
    if (tag == 0x3d && type == 3 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2() >> (14 - tiff_bps);
    if (tag == 0x81 && type == 4)
    {
      data_offset = get4();
      fseek(ifp, data_offset + 41, SEEK_SET);
      raw_height = get2() * 2;
      raw_width = get2();
      filters = 0x61616161;
    }
    if ((tag == 0x81 && type == 7)  ||
        (tag == 0x100 && type == 7 && strncmp(make, "NIKON", 5)) ||
        (tag == 0x280 && type == 1))
    {
      thumb_offset = ftell(ifp);
      thumb_length = len;
    }
    if (tag == 0x88 && type == 4 && (thumb_offset = get4()))
      thumb_offset += base;
    if (tag == 0x89 && type == 4)
      thumb_length = get4();
    if ((tag == 0x8c || tag == 0x96) && (type == 7))
      meta_offset = ftell(ifp);
    if ((tag == 0x97) && !strncmp(make, "NIKON", 5))
    {
      for (i = 0; i < 4; i++)
        ver97 = ver97 * 10 + fgetc(ifp) - '0';
      switch (ver97)
      {
      case 100:
        fseek(ifp, 68, SEEK_CUR);
        FORC4 cam_mul[(c >> 1) | ((c & 1) << 1)] = get2();
        break;
      case 102:
        fseek(ifp, 6, SEEK_CUR);
        FORC4 cam_mul[c ^ (c >> 1)] = get2();
        break;
      case 103:
        fseek(ifp, 16, SEEK_CUR);
        FORC4 cam_mul[c] = get2();
      }
      if (ver97 >= 200)
      {
        if (ver97 != 205)
          fseek(ifp, 280, SEEK_CUR);
        fread(buf97, 324, 1, ifp);
      }
    }
    if ((tag == 0xa1) && (type == 7) && strncasecmp(make, "Samsung", 7))
    {
      order = 0x4949;
      fseek(ifp, 140, SEEK_CUR);
      FORC3 cam_mul[c] = get4();
    }
    if (tag == 0xa4 && type == 3)
    {
      fseek(ifp, wbi * 48, SEEK_CUR);
      FORC3 cam_mul[c] = get2();
    }

    if (tag == 0xa7)
    { // shutter count
      NikonKey = fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp) ^ fgetc(ifp);
      if ((unsigned)(ver97 - 200) < 18)
      {
        ci = xlat[0][serial & 0xff];
        cj = xlat[1][NikonKey];
        ck = 0x60;
        for (i = 0; i < 324; i++)
          buf97[i] ^= (cj += ci * ck++);
        i = "66666>666;6A;:;555"[ver97 - 200] - '0';
        FORC4 cam_mul[c ^ (c >> 1) ^ (i & 1)] = sget2(buf97 + (i & -2) + c * 2);
      }
    }

    if (tag == 0xb001 && type == 3) // Sony ModelID
    {
      unique_id = get2();
    }
    if (tag == 0x200 && len == 3)
      shot_order = (get4(), get4());
    if (tag == 0x200 && len == 4) // Pentax black level
      FORC4 cblack[c ^ c >> 1] = get2();
    if (tag == 0x201 && len == 4) // Pentax As Shot WB
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
    if (tag == 0x220 && type == 7)
      meta_offset = ftell(ifp);
    if (tag == 0x401 && type == 4 && len == 4)
      FORC4 cblack[c ^ c >> 1] = get4();
    if (tag == 0xe01)
    { /* Nikon Capture Note */
      order = 0x4949;
      fseek(ifp, 22, SEEK_CUR);
      for (offset = 22; offset + 22 < len; offset += 22 + i)
      {
        tag = get4();
        fseek(ifp, 14, SEEK_CUR);
        i = get4() - 4;
        if (tag == 0x76a43207)
          flip = get2();
        else
          fseek(ifp, i, SEEK_CUR);
      }
    }
    if (tag == 0xe80 && len == 256 && type == 7)
    {
      fseek(ifp, 48, SEEK_CUR);
      cam_mul[0] = get2() * 508 * 1.078 / 0x10000;
      cam_mul[2] = get2() * 382 * 1.173 / 0x10000;
    }
    if (tag == 0xf00 && type == 7)
    {
      if (len == 614)
        fseek(ifp, 176, SEEK_CUR);
      else if (len == 734 || len == 1502)
        fseek(ifp, 148, SEEK_CUR);
      else
        goto next;
      goto get2_256;
    }

#ifndef LIBRAW_LIBRARY_BUILD
    if (((tag == 0x1011 && len == 9) || tag == 0x20400200) && strcmp(software, "v757-71"))
      for (i = 0; i < 3; i++)
        FORC3 cmatrix[i][c] = ((short)get2()) / 256.0;

    if ((tag == 0x1012 || tag == 0x20400600) && len == 4)
      FORC4 cblack[c ^ c >> 1] = get2();
    if (tag == 0x1017 || tag == 0x20400100)
      cam_mul[0] = get2() / 256.0;
    if (tag == 0x1018 || tag == 0x20400100)
      cam_mul[2] = get2() / 256.0;
#endif

    if (tag == 0x2011 && len == 2) {
get2_256:
      order = 0x4d4d;
      cam_mul[0] = get2() / 256.0;
      cam_mul[2] = get2() / 256.0;
    }
#ifndef LIBRAW_LIBRARY_BUILD
    if ((tag | 0x70) == 0x2070 && (type == 4 || type == 13))
      fseek(ifp, get4() + base, SEEK_SET);
    if ((tag == 0x2020) && ((type == 7) || (type == 13)) && !strncmp(buf, "OLYMP", 5))
      parse_thumb_note(base, 257, 258);
    if (tag == 0xb028) {
      fseek(ifp, get4() + base, SEEK_SET);
      parse_thumb_note(base, 136, 137);
    }
    if (tag == 0x2040)
      parse_makernote(base, 0x2040);
#endif
    if (tag == 0x4001 && len > 500 && len < 100000)
    {
      i = len == 582 ? 50 : len == 653 ? 68 : len == 5120 ? 142 : 126;
      fseek(ifp, i, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
      for (i += 18; i <= len; i += 10)
      {
        get2();
        FORC4 sraw_mul[c ^ (c >> 1)] = get2();
        if (sraw_mul[1] == 1170)
          break;
      }
    }
#ifndef LIBRAW_LIBRARY_BUILD
    if (!strncasecmp(make, "Samsung", 7))
    {
      if (tag == 0xa020) // get the full Samsung encryption key
        for (i = 0; i < 11; i++)
          SamsungKey[i] = get4();

      if (tag == 0xa021) // get and decode Samsung cam_mul array
        FORC4 cam_mul[c ^ (c >> 1)] = get4() - SamsungKey[c];

      if (tag == 0xa028)
        FORC4 cblack[c ^ (c >> 1)] = get4() - SamsungKey[c];

      if (tag == 0xa031 && len == 9) // get and decode Samsung color matrix
        for (i = 0; i < 3; i++)
          FORC3 cmatrix[i][c] = (float)((short)((get4() + SamsungKey[i * 3 + c]))) / 256.0;

    }
#endif
    if (strncasecmp(make, "Samsung", 7))
    {
      // Somebody else use 0xa021 and 0xa028?
      if (tag == 0xa021)
        FORC4 cam_mul[c ^ (c >> 1)] = get4();
      if (tag == 0xa028)
        FORC4 cam_mul[c ^ (c >> 1)] -= get4();
    }
#ifdef LIBRAW_LIBRARY_BUILD
    if (tag == 0x4021 && (imgdata.makernotes.canon.multishot[0] = get4()) &&
        (imgdata.makernotes.canon.multishot[1] = get4()))
    {
      if (len >= 4)
      {
        imgdata.makernotes.canon.multishot[2] = get4();
        imgdata.makernotes.canon.multishot[3] = get4();
      }
      FORC4 cam_mul[c] = 1024;
    }
#else
    if (tag == 0x4021 && get4() && get4())
      FORC4 cam_mul[c] = 1024;
#endif
  next:
    fseek(ifp, save, SEEK_SET);
  }
quit:
  order = sorder;
}