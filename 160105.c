void LibRaw::parse_gps(int base)
{
  unsigned entries, tag, type, len, save, c;

  entries = get2();
  if (entries > 40)
    return;
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
    case 0x0003:
    case 0x0005:
      gpsdata[29 + tag / 2] = getc(ifp);
      break;
    case 0x0002:
    case 0x0004:
    case 0x0007:
      FORC(6) gpsdata[tag / 3 * 6 + c] = get4();
      break;
    case 0x0006:
      FORC(2) gpsdata[18 + c] = get4();
      break;
    case 0x0012: // 18
    case 0x001d: // 29
      fgets((char *)(gpsdata + 14 + tag / 3), MIN(len, 12), ifp);
    }
    fseek(ifp, save, SEEK_SET);
  }
}