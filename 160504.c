void LibRaw::parseAdobePanoMakernote()
{
  uchar *PrivateMknBuf;
  unsigned posPrivateMknBuf;
  unsigned PrivateMknLength;
  unsigned PrivateOrder;
  unsigned PrivateEntries, PrivateTagID, PrivateTagType, PrivateTagCount;
  unsigned PrivateTagBytes;
  int truncated;

#define CHECKSPACE(s)                                                          \
  if (posPrivateMknBuf + (s) > PrivateMknLength)                               \
  {                                                                            \
    free(PrivateMknBuf);                                                       \
    return;                                                                    \
  }
#define icWBC imgdata.color.WB_Coeffs

  order = 0x4d4d;
  truncated = 0;
  PrivateMknLength = get4();

  if ((PrivateMknLength > 4) && (PrivateMknLength < 10240000) &&
      (PrivateMknBuf = (uchar *)malloc(PrivateMknLength + 1024)))
  { // 1024b for safety
    fread(PrivateMknBuf, PrivateMknLength, 1, ifp);
    PrivateOrder = sget2(PrivateMknBuf);
    PrivateEntries = sget2(PrivateMknBuf + 2);
    if ((PrivateEntries > 1000) ||
        ((PrivateOrder != 0x4d4d) && (PrivateOrder != 0x4949)))
    {
      free(PrivateMknBuf);
      return;
    }
    posPrivateMknBuf = 4;
    while (PrivateEntries--)
    {
      order = 0x4d4d;
      CHECKSPACE(8);
      PrivateTagID = sget2(PrivateMknBuf + posPrivateMknBuf);
      PrivateTagType = sget2(PrivateMknBuf + posPrivateMknBuf + 2);
      PrivateTagCount = sget4(PrivateMknBuf + posPrivateMknBuf + 4);
      posPrivateMknBuf += 8;
      order = PrivateOrder;

      if (truncated && !PrivateTagCount)
        continue;

      PrivateTagBytes =
          PrivateTagCount *
          ("11124811248484"[PrivateTagType < 14 ? PrivateTagType : 0] - '0');

      if (PrivateTagID == 0x0002)
      {
        posPrivateMknBuf += 2;
        CHECKSPACE(2);
        if (sget2(PrivateMknBuf + posPrivateMknBuf))
        {
          truncated = 1;
        }
        else
        {
          posPrivateMknBuf += 2;
        }
      }
      else if (PrivateTagID == 0x0013)
      {
        ushort nWB, cnt, tWB;
        CHECKSPACE(2);
        nWB = sget2(PrivateMknBuf + posPrivateMknBuf);
        posPrivateMknBuf += 2;
        if (nWB > 0x100)
          break;
        for (cnt = 0; cnt < nWB; cnt++)
        {
          CHECKSPACE(2);
          tWB = sget2(PrivateMknBuf + posPrivateMknBuf);
          if (tWB < 0x100)
          {
            CHECKSPACE(4);
            icWBC[tWB][0] = sget2(PrivateMknBuf + posPrivateMknBuf + 2);
            icWBC[tWB][2] = sget2(PrivateMknBuf + posPrivateMknBuf + 4);
            icWBC[tWB][1] = icWBC[tWB][3] = 0x100;
          }
          posPrivateMknBuf += 6;
        }
      }
      else if (PrivateTagID == 0x0027)
      {
        ushort nWB, cnt, tWB;
        CHECKSPACE(2);
        nWB = sget2(PrivateMknBuf + posPrivateMknBuf);
        posPrivateMknBuf += 2;
        if (nWB > 0x100)
          break;
        for (cnt = 0; cnt < nWB; cnt++)
        {
          CHECKSPACE(2);
          tWB = sget2(PrivateMknBuf + posPrivateMknBuf);
          if (tWB < 0x100)
          {
            CHECKSPACE(6);
            icWBC[tWB][0] = sget2(PrivateMknBuf + posPrivateMknBuf + 2);
            icWBC[tWB][1] = icWBC[tWB][3] =
                sget2(PrivateMknBuf + posPrivateMknBuf + 4);
            icWBC[tWB][2] = sget2(PrivateMknBuf + posPrivateMknBuf + 6);
          }
          posPrivateMknBuf += 8;
        }
      }
      else if (PrivateTagID == 0x0121)
      {
        CHECKSPACE(4);
        imPana.Multishot = sget4(PrivateMknBuf + posPrivateMknBuf);
        posPrivateMknBuf += 4;
      }
      else
      {
        if (PrivateTagBytes > 4)
          posPrivateMknBuf += PrivateTagBytes;
        else if (!truncated)
          posPrivateMknBuf += 4;
        else
        {
          if (PrivateTagBytes <= 2)
            posPrivateMknBuf += 2;
          else
            posPrivateMknBuf += 4;
        }
      }
    }
    free(PrivateMknBuf);
  }
#undef icWBC
#undef CHECKSPACE
}