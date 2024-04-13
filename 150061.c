static int IsBlank(int enc, int c)
{
  if (enc == ASCII)
    return isblank(c);

  if (c == 0x09	|| c == 0x20) return 1;

  switch (enc) {
  case UNICODE_ISO_8859_1:
  case ISO_8859_1:
  case ISO_8859_2:
  case ISO_8859_3:
  case ISO_8859_4:
  case ISO_8859_5:
  case ISO_8859_6:
  case ISO_8859_7:
  case ISO_8859_8:
  case ISO_8859_9:
  case ISO_8859_10:
  case ISO_8859_11:
  case ISO_8859_13:
  case ISO_8859_14:
  case ISO_8859_15:
  case ISO_8859_16:
  case KOI8:
    if (c == 0xa0) return 1;
    break;

  case KOI8_R:
    if (c == 0x9a) return 1;
    break;

  default:
    exit(-1);
  }

  return 0;
}