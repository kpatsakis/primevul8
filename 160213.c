void LibRaw::parse_gps_libraw(int base)
{
  unsigned entries, tag, type, len, save, c;

  entries = get2();
  if (entries > 40)
    return;
  if (entries > 0)
    imgdata.other.parsed_gps.gpsparsed = 1;
  while (entries--)
  {
    tiff_get(base, &tag, &type, &len, &save);
    if (len > 1024)
    {
      fseek(ifp, save, SEEK_SET); // Recover tiff-read position!!
      continue;                   // no GPS tags are 1k or larger
    }
    switch (tag)
    {
    case 0x0001:
      imgdata.other.parsed_gps.latref = getc(ifp);
      break;
    case 0x0003:
      imgdata.other.parsed_gps.longref = getc(ifp);
      break;
    case 0x0005:
      imgdata.other.parsed_gps.altref = getc(ifp);
      break;
    case 0x0002:
      if (len == 3)
        FORC(3) imgdata.other.parsed_gps.latitude[c] = getreal(type);
      break;
    case 0x0004:
      if (len == 3)
        FORC(3) imgdata.other.parsed_gps.longtitude[c] = getreal(type);
      break;
    case 0x0007:
      if (len == 3)
        FORC(3) imgdata.other.parsed_gps.gpstimestamp[c] = getreal(type);
      break;
    case 0x0006:
      imgdata.other.parsed_gps.altitude = getreal(type);
      break;
    case 0x0009:
      imgdata.other.parsed_gps.gpsstatus = getc(ifp);
      break;
    }
    fseek(ifp, save, SEEK_SET);
  }
}