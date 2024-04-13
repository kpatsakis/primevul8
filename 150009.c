static int IsPunct(int enc, int c)
{
  if (enc == ASCII)
    return ispunct(c);

  if (enc == UNICODE_ISO_8859_1) {
    if (c == 0x24 || c == 0x2b || c == 0x5e || c == 0x60 ||
        c == 0x7c || c == 0x7e) return 1;
    if (c >= 0x3c && c <= 0x3e) return 1;
  }

  if (c >= 0x21 && c <= 0x2f) return 1;
  if (c >= 0x3a && c <= 0x40) return 1;
  if (c >= 0x5b && c <= 0x60) return 1;
  if (c >= 0x7b && c <= 0x7e) return 1;

  switch (enc) {
  case ISO_8859_1:
  case ISO_8859_9:
  case ISO_8859_15:
    if (c == 0xad) return 1;
    /* fall */
  case UNICODE_ISO_8859_1:
    if (c == 0xa1) return 1;
    if (c == 0xab) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xbf) return 1;
    break;

  case ISO_8859_2:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_14:
    if (c == 0xad) return 1;
    break;

  case ISO_8859_3:
  case ISO_8859_10:
    if (c == 0xad) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbd) return 1;
    break;

  case ISO_8859_6:
    if (c == 0xac) return 1;
    if (c == 0xad) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xbf) return 1;
    break;

  case ISO_8859_7:
    if (c == 0xa1 || c == 0xa2) return 1;
    if (c == 0xab) return 1;
    if (c == 0xaf) return 1;
    if (c == 0xad) return 1;
    if (c == 0xb7 || c == 0xbb) return 1;
    break;

  case ISO_8859_8:
    if (c == 0xab) return 1;
    if (c == 0xad) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xdf) return 1;
    break;

  case ISO_8859_13:
    if (c == 0xa1 || c == 0xa5) return 1;
    if (c == 0xab || c == 0xad) return 1;
    if (c == 0xb4 || c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    if (c == 0xff) return 1;
    break;

  case ISO_8859_16:
    if (c == 0xa5) return 1;
    if (c == 0xab) return 1;
    if (c == 0xad) return 1;
    if (c == 0xb5) return 1;
    if (c == 0xb7) return 1;
    if (c == 0xbb) return 1;
    break;

  case KOI8_R:
    if (c == 0x9e) return 1;
    break;

  case ISO_8859_11:
  case KOI8:
    break;

  default:
    exit(-1);
  }

  return 0;
}