unicode_type_find (GstTypeFind * tf, const GstUnicodeTester * tester,
    guint n_tester, const char *media_type, gboolean require_bom)
{
  size_t n;
  gint len = 4;
  const guint8 *data = gst_type_find_peek (tf, 0, len);
  int prob = -1;
  const gint max_scan_size = 256 * 1024;
  int endianness = 0;

  if (!data) {
    len = 2;
    data = gst_type_find_peek (tf, 0, len);
    if (!data)
      return;
  }

  /* find a large enough size that works */
  while (len < max_scan_size) {
    size_t newlen = len << 1;
    const guint8 *newdata = gst_type_find_peek (tf, 0, newlen);
    if (!newdata)
      break;
    len = newlen;
    data = newdata;
  }

  for (n = 0; n < n_tester; ++n) {
    int bom_boost = 0, tmpprob;
    if (len >= tester[n].bomlen) {
      if (!memcmp (data, tester[n].bom, tester[n].bomlen))
        bom_boost = tester[n].boost;
    }
    if (require_bom && bom_boost == 0)
      continue;
    if (!(*tester[n].checker) (data, len, tester[n].endianness))
      continue;
    tmpprob = GST_TYPE_FIND_POSSIBLE - 20 + bom_boost;
    if (tmpprob > prob) {
      prob = tmpprob;
      endianness = tester[n].endianness;
    }
  }

  if (prob > 0) {
    GST_DEBUG ("This is valid %s %s", media_type,
        endianness == G_BIG_ENDIAN ? "be" : "le");
    gst_type_find_suggest_simple (tf, prob, media_type,
        "endianness", G_TYPE_INT, endianness, NULL);
  }
}