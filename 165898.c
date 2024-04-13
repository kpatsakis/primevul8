void CLASS parse_makernote_0xc634(int base, int uptag, unsigned dng_writer)
{

  if (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_SKIP_MAKERNOTES)
  	return;

  if (!strncmp(make, "NIKON", 5)) {
    parseNikonMakernote (base, uptag, AdobeDNG);
    return;
  } else if (!strncasecmp(make, "LEICA", 5)) {
    parseLeicaMakernote (base, uptag, is_0xc634);
    return;
  }

  unsigned offset = 0, entries, tag, type, len, save, c;
  unsigned i;

  uchar *CanonCameraInfo;
  unsigned lenCanonCameraInfo = 0;
  unsigned typeCanonCameraInfo = 0;

  int isSony = 0;

  int LeicaMakernoteSignature = -1;

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

  short morder, sorder = order;
  char buf[10];
  INT64 fsize = ifp->size();

  fread(buf, 1, 10, ifp);

  if (!strcmp(buf, "OLYMPUS") ||
      !strcmp(buf, "PENTAX ") ||
      (!strncmp(make, "SAMSUNG", 7) && (dng_writer == CameraDNG))) {
    base = ftell(ifp) - 10;
    fseek(ifp, -2, SEEK_CUR);
    order = get2();
    if (buf[0] == 'O') get2();

  } else if (!strncmp(buf, "SONY", 4) ||
             !strcmp(buf, "Panasonic")) {
    order = 0x4949;
    fseek(ifp, 2, SEEK_CUR);

  } else if (!strncmp(buf, "FUJIFILM", 8)) {
    base = ftell(ifp) - 10;
    order = 0x4949;
    fseek(ifp, 2, SEEK_CUR);

  } else if (!strcmp(buf, "OLYMP") ||
             !strcmp(buf, "Ricoh") ||
             !strcmp(buf, "EPSON")) {
    fseek(ifp, -2, SEEK_CUR);

  } else if (!strcmp(buf, "AOC") ||
             !strcmp(buf, "QVC")) {
    fseek(ifp, -4, SEEK_CUR);

  } else {
    fseek(ifp, -10, SEEK_CUR);
    if ((!strncmp(make, "SAMSUNG", 7) &&
        (dng_writer == AdobeDNG)))
      base = ftell(ifp);
  }

  entries = get2();
  if (entries > 1000) return;

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

  morder = order;
  while (entries--) {
    order = morder;
    tiff_get(base, &tag, &type, &len, &save);
    INT64 pos = ifp->tell();
    if (len > 8 && pos + len > 2 * fsize) {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;
    }
    tag |= uptag << 16;
    if (len > 100 * 1024 * 1024)
      goto next; // 100Mb tag? No!

    if (!strncmp(make, "Canon", 5)) {
      if (tag == 0x000d && len < 256000) { // camera info
        if (type != 4) {
          CanonCameraInfo = (uchar *)malloc(MAX(16, len));
          fread(CanonCameraInfo, len, 1, ifp);

        } else {
          CanonCameraInfo = (uchar *)malloc(MAX(16, len * 4));
          fread(CanonCameraInfo, len, 4, ifp);
        }
        lenCanonCameraInfo = len;
        typeCanonCameraInfo = type;
      }

      else if (tag == 0x10) { // Canon ModelID
        unique_id = get4();
        unique_id = setCanonBodyFeatures(unique_id);
        if (lenCanonCameraInfo) {
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
      parseFujiMakernotes(tag, type, len, AdobeDNG);

    else if (!strncmp(make, "OLYMPUS", 7)) {

      int SubDirOffsetValid =
          strncmp(model, "E-300", 5) &&
          strncmp(model, "E-330", 5) &&
          strncmp(model, "E-400", 5) &&
          strncmp(model, "E-500", 5) &&
          strncmp(model, "E-1", 3);

      if ((tag == 0x2010) ||
          (tag == 0x2020) ||
          (tag == 0x2030) ||
          (tag == 0x2031) ||
          (tag == 0x2040) ||
          (tag == 0x2050) ||
          (tag == 0x3000)) {
        fseek(ifp, save - 4, SEEK_SET);
        fseek(ifp, base + get4(), SEEK_SET);
        parse_makernote_0xc634(base, tag, dng_writer);
      }

      if (!SubDirOffsetValid &&
          ((len > 4) ||
           (((type == 3) || (type == 8)) && (len > 2)) ||
           (((type == 4) || (type == 9)) && (len > 1)) ||
           (type == 5) ||
           (type > 9)))
        goto skip_Oly_broken_tags;

      else if ((tag >= 0x20100000) && (tag <= 0x2010ffff))
        parseOlympus_Equipment ((tag & 0x0000ffff), type, len, AdobeDNG);

      else if ((tag >= 0x20200000) && (tag <= 0x2020ffff))
        parseOlympus_CameraSettings (base, (tag & 0x0000ffff), type, len, AdobeDNG);

      else if ((tag == 0x20300108) || (tag == 0x20310109))
        imgdata.makernotes.olympus.ColorSpace = get2();

      else if ((tag >= 0x20400000) && (tag <= 0x2040ffff))
        parseOlympus_ImageProcessing ((tag & 0x0000ffff), type, len, AdobeDNG);

      else if ((tag >= 0x30000000) && (tag <= 0x3000ffff))
        parseOlympus_RawInfo ((tag & 0x0000ffff), type, len, AdobeDNG);

      else switch (tag) {
      case 0x0207:
        getOlympus_CameraType2();
        break;
      case 0x1002:
        imgdata.lens.makernotes.CurAp = libraw_powf64l(2.0f, getreal(type) / 2);
        break;
      case 0x1007:
        imgdata.other.SensorTemperature = (float)get2();
        break;
      case 0x1008:
        imgdata.other.LensTemperature = (float)get2();
        break;
      case 0x20501500:
        getOlympus_SensorTemperature(len);
        break;
      }

skip_Oly_broken_tags:;
    }

    else if (!strncmp(make, "PENTAX", 6) ||
             !strncmp(model, "PENTAX", 6)) {
      parsePentaxMakernotes(base, tag, type, len, dng_writer);

    } else if (!strncmp(make, "SAMSUNG", 7)) {
      if (dng_writer == AdobeDNG)
        parseSamsungMakernotes(base, tag, type, len, dng_writer);
      else
        parsePentaxMakernotes(base, tag, type, len, dng_writer);

    } else if (isSony) {
      parseSonyMakernotes(base, tag, type, len, AdobeDNG,
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
  next:
    fseek(ifp, save, SEEK_SET);
  }
quit:
  order = sorder;
}