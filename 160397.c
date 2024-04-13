int LibRaw::parseLeicaInternalBodySerial(unsigned len)
{
#define plibs imgdata.shootinginfo.InternalBodySerial
  if (!len)
  {
    strcpy(plibs, "N/A");
    return 0;
  }
  stmread(plibs, len, ifp);
  if (!strncmp(plibs, "000000000000", 12))
  {
    plibs[0] = '0';
    plibs[1] = '\0';
    return 1;
  }

  if (strnlen(plibs, len) == 13)
  {
    for (int i = 3; i < 13; i++)
    {
      if (!isdigit(plibs[i]))
        goto non_std;
    }
    memcpy(plibs + 15, plibs + 9, 4);
    memcpy(plibs + 12, plibs + 7, 2);
    memcpy(plibs + 9, plibs + 5, 2);
    memcpy(plibs + 6, plibs + 3, 2);
    plibs[3] = plibs[14] = ' ';
    plibs[8] = plibs[11] = '/';
    if (((short)(plibs[3] - '0') * 10 + (short)(plibs[4] - '0')) < 70)
    {
      memcpy(plibs + 4, "20", 2);
    }
    else
    {
      memcpy(plibs + 4, "19", 2);
    }
    return 2;
  }
non_std:
#undef plibs
  return 1;
}