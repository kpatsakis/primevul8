multipart_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data;
  const guint8 *x;

#define MULTIPART_MAX_BOUNDARY_OFFSET 16
  data = gst_type_find_peek (tf, 0, MULTIPART_MAX_BOUNDARY_OFFSET);
  if (!data)
    return;

  for (x = data;
      x - data < MULTIPART_MAX_BOUNDARY_OFFSET - 2 && g_ascii_isspace (*x);
      x++);
  if (x[0] != '-' || x[1] != '-')
    return;

  /* Could be okay, peek what should be enough for a complete header */
#define MULTIPART_MAX_HEADER_SIZE 256
  data = gst_type_find_peek (tf, 0, MULTIPART_MAX_HEADER_SIZE);
  if (!data)
    return;

  for (x = data; x - data < MULTIPART_MAX_HEADER_SIZE - 14; x++) {
    if (!isascii (*x)) {
      return;
    }
    if (*x == '\n' &&
        !g_ascii_strncasecmp ("content-type:", (gchar *) x + 1, 13)) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MULTIPART_CAPS);
      return;
    }
  }
}