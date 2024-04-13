char *_poppler_goo_string_to_utf8(const GooString *s)
{
  if (s == nullptr) {
    return nullptr;
  }

  char *result;

  if (s->hasUnicodeMarker()) {
    result = g_convert (s->getCString () + 2,
			s->getLength () - 2,
			"UTF-8", "UTF-16BE", nullptr, nullptr, nullptr);
  } else {
    int len;
    gunichar *ucs4_temp;
    int i;
    
    len = s->getLength ();
    ucs4_temp = g_new (gunichar, len + 1);
    for (i = 0; i < len; ++i) {
      ucs4_temp[i] = pdfDocEncoding[(unsigned char)s->getChar(i)];
    }
    ucs4_temp[i] = 0;

    result = g_ucs4_to_utf8 (ucs4_temp, -1, nullptr, nullptr, nullptr);

    g_free (ucs4_temp);
  }

  return result;
}