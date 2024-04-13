static int IsPrint(int enc, int c)
{
  if (enc == ASCII)
    return isprint(c);

  if (c >= 0x20 && c <= 0x7e) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
    /* if (c >= 0x09 && c <= 0x0d) return 1; */
    if (c == 0x85) return 1;
    /* fall */
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
    if (c >= 0xa0 && c <= 0xff) return 1;
    break;

  case ISO_8859_3:
    if (c >= 0xa0) {
      if (c == 0xa5 || c == 0xae || c == 0xbe || c == 0xc3 || c == 0xd0 ||
	  c == 0xe3 || c == 0xf0)
	return 0;
      else
	return 1;
    }
    break;

  case ISO_8859_6:
    if (c == 0xa0) return 1;
    if (c == 0xa4 || c == 0xac || c == 0xad || c == 0xbb || c == 0xbf)
      return 1;
    if (c >= 0xc1 && c <= 0xda) return 1;
    if (c >= 0xe0 && c <= 0xf2) return 1;
    break;

  case ISO_8859_7:
    if (c >= 0xa0 && c <= 0xfe &&
	c != 0xa4 && c != 0xa5 && c != 0xaa &&
	c != 0xae && c != 0xd2) return 1;
    break;

  case ISO_8859_8:
    if (c >= 0xa0 && c <= 0xfa) {
      if (c >= 0xbf && c <= 0xde) return 0;
      if (c == 0xa1) return 0;
      return 1;
    }
    break;

  case ISO_8859_11:
    if (c >= 0xa0 && c <= 0xda) return 1;
    if (c >= 0xdf && c <= 0xfb) return 1;
    break;

  case KOI8:
    if (c == 0xa0) return 1;
    if (c >= 0xc0 && c <= 0xff) return 1;
    break;

  case KOI8_R:
    if (c >= 0x80 && c <= 0xff) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}