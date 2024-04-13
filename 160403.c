void LibRaw::PentaxLensInfo(unsigned long long id, unsigned len) // tag 0x0207
{
  ushort iLensData = 0;
  uchar *table_buf;
  table_buf = (uchar *)malloc(MAX(len, 128));
  fread(table_buf, len, 1, ifp);
  if ((id < 0x12b9c) || (((id == 0x12b9cULL) ||  // K100D
                          (id == 0x12b9dULL) ||  // K110D
                          (id == 0x12ba2ULL)) && // K100D Super
                         ((!table_buf[20] || (table_buf[20] == 0xff)))))
  {
    iLensData = 3;
    if (ilm.LensID == -1)
      ilm.LensID = (((unsigned)table_buf[0]) << 8) + table_buf[1];
  }
  else
    switch (len)
    {
    case 90: // LensInfo3
      iLensData = 13;
      if (ilm.LensID == -1)
        ilm.LensID = ((unsigned)((table_buf[1] & 0x0f) + table_buf[3]) << 8) +
                     table_buf[4];
      break;
    case 91: // LensInfo4
      iLensData = 12;
      if (ilm.LensID == -1)
        ilm.LensID = ((unsigned)((table_buf[1] & 0x0f) + table_buf[3]) << 8) +
                     table_buf[4];
      break;
    case 80: // LensInfo5
    case 128:
      iLensData = 15;
      if (ilm.LensID == -1)
        ilm.LensID = ((unsigned)((table_buf[1] & 0x0f) + table_buf[4]) << 8) +
                     table_buf[5];
      break;
    case 168: // Ricoh GR III, id 0x1320e
      break;
    default:
      if (id >= 0x12b9cULL) // LensInfo2
      {
        iLensData = 4;
        if (ilm.LensID == -1)
          ilm.LensID = ((unsigned)((table_buf[0] & 0x0f) + table_buf[2]) << 8) +
                       table_buf[3];
      }
    }
  if (iLensData)
  {
    if (table_buf[iLensData + 9] && (fabs(ilm.CurFocal) < 0.1f))
      ilm.CurFocal = 10 * (table_buf[iLensData + 9] >> 2) *
                     libraw_powf64l(4, (table_buf[iLensData + 9] & 0x03) - 2);
    if (table_buf[iLensData + 10] & 0xf0)
      ilm.MaxAp4CurFocal = libraw_powf64l(
          2.0f, (float)((table_buf[iLensData + 10] & 0xf0) >> 4) / 4.0f);
    if (table_buf[iLensData + 10] & 0x0f)
      ilm.MinAp4CurFocal = libraw_powf64l(
          2.0f, (float)((table_buf[iLensData + 10] & 0x0f) + 10) / 4.0f);

    if (iLensData != 12)
    {
      switch (table_buf[iLensData] & 0x06)
      {
      case 0:
        ilm.MinAp4MinFocal = 22.0f;
        break;
      case 2:
        ilm.MinAp4MinFocal = 32.0f;
        break;
      case 4:
        ilm.MinAp4MinFocal = 45.0f;
        break;
      case 6:
        ilm.MinAp4MinFocal = 16.0f;
        break;
      }
      if (table_buf[iLensData] & 0x70)
        ilm.LensFStops =
            ((float)(((table_buf[iLensData] & 0x70) >> 4) ^ 0x07)) / 2.0f +
            5.0f;

      ilm.MinFocusDistance = (float)(table_buf[iLensData + 3] & 0xf8);
      ilm.FocusRangeIndex = (float)(table_buf[iLensData + 3] & 0x07);

      if ((table_buf[iLensData + 14] > 1) && (fabs(ilm.MaxAp4CurFocal) < 0.7f))
        ilm.MaxAp4CurFocal = libraw_powf64l(
            2.0f, (float)((table_buf[iLensData + 14] & 0x7f) - 1) / 32.0f);
    }
    else if ((id != 0x12e76ULL) && // K-5
             (table_buf[iLensData + 15] > 1) &&
             (fabs(ilm.MaxAp4CurFocal) < 0.7f))
    {
      ilm.MaxAp4CurFocal = libraw_powf64l(
          2.0f, (float)((table_buf[iLensData + 15] & 0x7f) - 1) / 32.0f);
    }
  }
  free(table_buf);
  return;
}