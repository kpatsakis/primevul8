void LibRaw::parseSonySRF(unsigned len)
{

  if ((len > 0xfffff) || (len == 0))
    return;

  INT64 save = ftell(ifp);
  INT64 offset =
      0x0310c0 - save; /* for non-DNG this value normally is 0x8ddc */
  if (len < offset || offset < 0)
    return;
  INT64 decrypt_len = offset >> 2; /* master key offset value is the next
                                      un-encrypted metadata field after SRF0 */

  unsigned i;
  unsigned MasterKey, SRF2Key, RawDataKey;
  INT64 srf_offset;
  uchar *srf_buf;
  short entries;
  unsigned tag_id, tag_type, tag_datalen, tag_val;

  srf_buf = (uchar *)malloc(len);
  fread(srf_buf, len, 1, ifp);

  offset += srf_buf[offset] << 2;

#define CHECKBUFFER_SGET4(offset)                                              \
  do                                                                           \
  {                                                                            \
    if ((offset + 4) > len)                                                    \
      goto restore_after_parseSonySRF;                                         \
  } while (0)
#define CHECKBUFFER_SGET2(offset)                                              \
  do                                                                           \
  {                                                                            \
    if ((offset + 2) > len)                                                    \
      goto restore_after_parseSonySRF;                                         \
  } while (0)

  CHECKBUFFER_SGET4(offset);

  /* master key is stored in big endian */
  MasterKey = ((unsigned)srf_buf[offset] << 24) |
              ((unsigned)srf_buf[offset + 1] << 16) |
              ((unsigned)srf_buf[offset + 2] << 8) |
              (unsigned)srf_buf[offset + 3];

  /* skip SRF0 */
  srf_offset = 0;
  CHECKBUFFER_SGET2(srf_offset);
  entries = sget2(srf_buf + srf_offset);
  if (entries > 1000)
    goto restore_after_parseSonySRF;
  offset = srf_offset + 2;
  CHECKBUFFER_SGET4(offset + 12 * entries);
  srf_offset = sget4(srf_buf + offset + 12 * entries) -
               save; /* SRF0 ends with SRF1 abs. position */

  /* get SRF1, it has fixed 40 bytes length and contains keys to decode metadata
   * and raw data */
  if (srf_offset < 0 || decrypt_len < srf_offset / 4)
    goto restore_after_parseSonySRF;
  sony_decrypt((unsigned *)(srf_buf + srf_offset), decrypt_len - srf_offset / 4,
               1, MasterKey);
  CHECKBUFFER_SGET2(srf_offset);
  entries = sget2(srf_buf + srf_offset);
  if (entries > 1000)
    goto restore_after_parseSonySRF;
  offset = srf_offset + 2;

  while (entries--)
  {
    CHECKBUFFER_SGET4(srf_offset + 8);
    tag_id = sget2(srf_buf + offset);
    tag_type = sget2(srf_buf + offset + 2);
    tag_datalen = sget4(srf_buf + offset + 4);
    tag_val = sget4(srf_buf + offset + 8);
    if (tag_id == 0x0000)
      SRF2Key = tag_val;
    else if (tag_id == 0x0001)
      RawDataKey = tag_val;
    offset += 12;
  }

  /* get SRF2 */
  CHECKBUFFER_SGET4(offset);
  srf_offset =
      sget4(srf_buf + offset) - save; /* SRFn ends with SRFn+1 position */
  if (srf_offset < 0 || decrypt_len < srf_offset / 4)
    goto restore_after_parseSonySRF;
  sony_decrypt((unsigned *)(srf_buf + srf_offset), decrypt_len - srf_offset / 4,
               1, SRF2Key);
  CHECKBUFFER_SGET2(srf_offset);
  entries = sget2(srf_buf + srf_offset);
  if (entries > 1000)
    goto restore_after_parseSonySRF;
  offset = srf_offset + 2;
  while (entries--)
  {
    CHECKBUFFER_SGET4(srf_offset + 8);
    tag_id = sget2(srf_buf + offset);
    tag_type = sget2(srf_buf + offset + 2);
    tag_datalen = sget4(srf_buf + offset + 4);
    tag_val = sget4(srf_buf + offset + 8);
    switch (tag_id)
    {
      /*
      0x0002	SRF6Offset
      0x0003	SRFDataOffset (?)
      0x0004	RawDataOffset
      0x0005	RawDataLength
      */
    case 0x0043:
      CHECKBUFFER_SGET4(tag_val - save + 4);
      i = sget4(srf_buf + tag_val - save + 4);
      // next sget4 already checked b/c offset is less
      if (i)
        ilm.MaxAp4MaxFocal =
            ((float)sget4(srf_buf + tag_val - save)) / ((float)i);
      break;
    case 0x0044:
      CHECKBUFFER_SGET4(tag_val - save + 4);
      i = sget4(srf_buf + tag_val - save + 4);
      if (i)
        ilm.MaxAp4MinFocal =
            ((float)sget4(srf_buf + tag_val - save)) / ((float)i);
      break;
    case 0x0045:
      CHECKBUFFER_SGET4(tag_val - save + 4);
      i = sget4(srf_buf + tag_val - save + 4);
      if (i)
        ilm.MinFocal = ((float)sget4(srf_buf + tag_val - save)) / ((float)i);
      break;
    case 0x0046:
      CHECKBUFFER_SGET4(tag_val - save + 4);
      i = sget4(srf_buf + tag_val - save + 4);
      if (i)
        ilm.MaxFocal = ((float)sget4(srf_buf + tag_val - save)) / ((float)i);
      break;

    case 0x00c0:
      icWBC[LIBRAW_WBI_Daylight][0] = tag_val;
      break;
    case 0x00c1:
      icWBC[LIBRAW_WBI_Daylight][1] = icWBC[LIBRAW_WBI_Daylight][3] = tag_val;
      break;
    case 0x00c2:
      icWBC[LIBRAW_WBI_Daylight][2] = tag_val;
      break;
    case 0x00c3:
      icWBC[LIBRAW_WBI_Cloudy][0] = tag_val;
      break;
    case 0x00c4:
      icWBC[LIBRAW_WBI_Cloudy][1] = icWBC[LIBRAW_WBI_Cloudy][3] = tag_val;
      break;
    case 0x00c5:
      icWBC[LIBRAW_WBI_Cloudy][2] = tag_val;
      break;
    case 0x00c6:
      icWBC[LIBRAW_WBI_Fluorescent][0] = tag_val;
      break;
    case 0x00c7:
      icWBC[LIBRAW_WBI_Fluorescent][1] = icWBC[LIBRAW_WBI_Fluorescent][3] =
          tag_val;
      break;
    case 0x00c8:
      icWBC[LIBRAW_WBI_Fluorescent][2] = tag_val;
      break;
    case 0x00c9:
      icWBC[LIBRAW_WBI_Tungsten][0] = tag_val;
      break;
    case 0x00ca:
      icWBC[LIBRAW_WBI_Tungsten][1] = icWBC[LIBRAW_WBI_Tungsten][3] = tag_val;
      break;
    case 0x00cb:
      icWBC[LIBRAW_WBI_Tungsten][2] = tag_val;
      break;
    case 0x00cc:
      icWBC[LIBRAW_WBI_Flash][0] = tag_val;
      break;
    case 0x00cd:
      icWBC[LIBRAW_WBI_Flash][1] = icWBC[LIBRAW_WBI_Flash][3] = tag_val;
      break;
    case 0x00ce:
      icWBC[LIBRAW_WBI_Flash][2] = tag_val;
      break;
    case 0x00d0:
      cam_mul[0] = tag_val;
      break;
    case 0x00d1:
      cam_mul[1] = cam_mul[3] = tag_val;
      break;
    case 0x00d2:
      cam_mul[2] = tag_val;
      break;
    }
    offset += 12;
  }

restore_after_parseSonySRF:
  free(srf_buf);
  fseek(ifp, save, SEEK_SET);
#undef CHECKBUFFER_SGET4
#undef CHECKBUFFER_SGET2
}