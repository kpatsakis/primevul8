utf8_type_find_have_valid_utf8_at_offset (GstTypeFind * tf, guint64 offset,
    GstTypeFindProbability * prob)
{
  const guint8 *data;

  /* randomly decided values */
  guint min_size = 16;          /* minimum size  */
  guint size = 32 * 1024;       /* starting size */
  guint probability = 95;       /* starting probability */
  guint step = 10;              /* how much we reduce probability in each
                                 * iteration */

  while (probability > step && size > min_size) {
    data = gst_type_find_peek (tf, offset, size);
    if (data) {
      gchar *end;
      gchar *start = (gchar *) data;

      if (g_utf8_validate (start, size, (const gchar **) &end) || (end - start + 4 > size)) {   /* allow last char to be cut off */
        *prob = probability;
        return TRUE;
      }
      *prob = 0;
      return FALSE;
    }
    size /= 2;
    probability -= step;
  }
  *prob = 0;
  return FALSE;
}