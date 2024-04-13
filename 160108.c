void LibRaw::parseSonySR2(uchar *cbuf_SR2, unsigned SR2SubIFDOffset,
                          unsigned SR2SubIFDLength, unsigned dng_writer)
{
  unsigned c;
  unsigned entries, tag, type, len;
  unsigned icbuf_SR2;
  int ival;
  int TagProcessed;
  float num;
  int i;
  entries = sget2(cbuf_SR2);
  if (entries > 1000)
    return;
  icbuf_SR2 = 2;
  while (entries--)
  {
    tag = sget2(cbuf_SR2 + icbuf_SR2);
    icbuf_SR2 += 2;
    type = sget2(cbuf_SR2 + icbuf_SR2);
    icbuf_SR2 += 2;
    len = sget4(cbuf_SR2 + icbuf_SR2);
    icbuf_SR2 += 4;

    if (len * ("11124811248484"[type < 14 ? type : 0] - '0') > 4)
    {
      ival = sget4(cbuf_SR2 + icbuf_SR2) - SR2SubIFDOffset;
    }
    else
    {
      ival = icbuf_SR2;
    }
    if (ival > SR2SubIFDLength) // points out of orig. buffer size
      break; // END processing. Generally we should check against
             // SR2SubIFDLength minus 6 of 8, depending on tag, but we allocated
             // extra 1024b for buffer, so this does not matter

    icbuf_SR2 += 4;
    TagProcessed = 0;

    if (dng_writer == nonDNG)
    {
      switch (tag)
      {
      case 0x7300:
        for (c = 0; c < 4 && c < len; c++)
          cblack[c] = sget2(cbuf_SR2 + ival + 2 * c);
        TagProcessed = 1;
        break;
      case 0x7303:
        FORC4 cam_mul[c ^ (c < 2)] = sget2(cbuf_SR2 + ival + 2 * c);
        TagProcessed = 1;
        break;
      case 0x7310:
        FORC4 cblack[c ^ c >> 1] = sget2(cbuf_SR2 + ival + 2 * c);
        i = cblack[3];
        FORC3 if (i > cblack[c]) i = cblack[c];
        FORC4 cblack[c] -= i;
        black = i;
        TagProcessed = 1;
        break;
      case 0x7313:
        FORC4 cam_mul[c ^ (c >> 1)] = sget2(cbuf_SR2 + ival + 2 * c);
        TagProcessed = 1;
        break;
      case 0x74a0:
        c = sget4(cbuf_SR2 + ival + 4);
        if (c)
          ilm.MaxAp4MaxFocal = ((float)sget4(cbuf_SR2 + ival)) / ((float)c);
        TagProcessed = 1;
        break;
      case 0x74a1:
        c = sget4(cbuf_SR2 + ival + 4);
        if (c)
          ilm.MaxAp4MinFocal = ((float)sget4(cbuf_SR2 + ival)) / ((float)c);
        TagProcessed = 1;
        break;
      case 0x74a2:
        c = sget4(cbuf_SR2 + ival + 4);
        if (c)
          ilm.MaxFocal = ((float)sget4(cbuf_SR2 + ival)) / ((float)c);
        TagProcessed = 1;
        break;
      case 0x74a3:
        c = sget4(cbuf_SR2 + ival + 4);
        if (c)
          ilm.MinFocal = ((float)sget4(cbuf_SR2 + ival)) / ((float)c);
        TagProcessed = 1;
        break;
      case 0x7800:
        for (i = 0; i < 3; i++)
        {
          num = 0.0;
          for (c = 0; c < 3; c++)
          {
            imgdata.color.ccm[i][c] =
                (float)((short)sget2(cbuf_SR2 + ival + 2 * (i * 3 + c)));
            num += imgdata.color.ccm[i][c];
          }
          if (num > 0.01)
            FORC3 imgdata.color.ccm[i][c] = imgdata.color.ccm[i][c] / num;
        }
        TagProcessed = 1;
        break;
      case 0x787f:
        if (len == 3)
        {
          FORC3 imgdata.color.linear_max[c] = sget2(cbuf_SR2 + ival + 2 * c);
          imgdata.color.linear_max[3] = imgdata.color.linear_max[1];
        }
        else if (len == 1)
        {
          imgdata.color.linear_max[0] = imgdata.color.linear_max[1] =
              imgdata.color.linear_max[2] = imgdata.color.linear_max[3] =
                  sget2(cbuf_SR2 + ival);
        }
        TagProcessed = 1;
        break;
      }
    }

    if (!TagProcessed)
    {
      switch (tag)
      {
      case 0x7302:
        FORC4 icWBC[LIBRAW_WBI_Auto][c ^ (c < 2)] =
            sget2(cbuf_SR2 + ival + 2 * c);
        break;
      case 0x7312:
        FORC4 icWBC[LIBRAW_WBI_Auto][c ^ (c >> 1)] =
            sget2(cbuf_SR2 + ival + 2 * c);
        break;
      case 0x7480:
      case 0x7820:
        FORC3 icWBC[LIBRAW_WBI_Daylight][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_Daylight][3] = icWBC[LIBRAW_WBI_Daylight][1];
        break;
      case 0x7481:
      case 0x7821:
        FORC3 icWBC[LIBRAW_WBI_Cloudy][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_Cloudy][3] = icWBC[LIBRAW_WBI_Cloudy][1];
        break;
      case 0x7482:
      case 0x7822:
        FORC3 icWBC[LIBRAW_WBI_Tungsten][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_Tungsten][3] = icWBC[LIBRAW_WBI_Tungsten][1];
        break;
      case 0x7483:
      case 0x7823:
        FORC3 icWBC[LIBRAW_WBI_Flash][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_Flash][3] = icWBC[LIBRAW_WBI_Flash][1];
        break;
      case 0x7484:
      case 0x7824:
        icWBCTC[0][0] = 4500;
        FORC3 icWBCTC[0][c + 1] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBCTC[0][4] = icWBCTC[0][2];
        break;
      case 0x7486:
        FORC3 icWBC[LIBRAW_WBI_Fluorescent][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_Fluorescent][3] = icWBC[LIBRAW_WBI_Fluorescent][1];
        break;
      case 0x7825:
        FORC3 icWBC[LIBRAW_WBI_Shade][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_Shade][3] = icWBC[LIBRAW_WBI_Shade][1];
        break;
      case 0x7826:
        FORC3 icWBC[LIBRAW_WBI_FL_W][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_FL_W][3] = icWBC[LIBRAW_WBI_FL_W][1];
        break;
      case 0x7827:
        FORC3 icWBC[LIBRAW_WBI_FL_N][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_FL_N][3] = icWBC[LIBRAW_WBI_FL_N][1];
        break;
      case 0x7828:
        FORC3 icWBC[LIBRAW_WBI_FL_D][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_FL_D][3] = icWBC[LIBRAW_WBI_FL_D][1];
        break;
      case 0x7829:
        FORC3 icWBC[LIBRAW_WBI_FL_L][c] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_FL_L][3] = icWBC[LIBRAW_WBI_FL_L][1];
        break;
      case 0x782a:
        icWBCTC[1][0] = 8500;
        FORC3 icWBCTC[1][c + 1] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBCTC[1][4] = icWBCTC[1][2];
        break;
      case 0x782b:
        icWBCTC[2][0] = 6000;
        FORC3 icWBCTC[2][c + 1] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBCTC[2][4] = icWBCTC[2][2];
        break;
      case 0x782c:
        icWBCTC[3][0] = 3200;
        FORC3 icWBC[LIBRAW_WBI_StudioTungsten][c] = icWBCTC[3][c + 1] =
            sget2(cbuf_SR2 + ival + 2 * c);
        icWBC[LIBRAW_WBI_StudioTungsten][3] = icWBCTC[3][4] = icWBCTC[3][2];
        ;
        break;
      case 0x782d:
        icWBCTC[4][0] = 2500;
        FORC3 icWBCTC[4][c + 1] = sget2(cbuf_SR2 + ival + 2 * c);
        icWBCTC[4][4] = icWBCTC[4][2];
        break;
      }
    }
  }
}