uri_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 0, BUFFER_SIZE);
  guint pos = 0;
  guint offset = 0;

  if (data) {
    /* Search for # comment lines */
    while (*data == '#') {
      /* Goto end of line */
      while (*data != '\n') {
        INC_BUFFER;
      }

      INC_BUFFER;
    }

    if (!g_ascii_isalpha (*data)) {
      /* Had a non alpha char - can't be uri-list */
      return;
    }

    INC_BUFFER;

    while (g_ascii_isalnum (*data)) {
      INC_BUFFER;
    }

    if (*data != ':') {
      /* First non alpha char is not a : */
      return;
    }

    /* Get the next 2 bytes as well */
    data = gst_type_find_peek (tf, offset + pos, 3);
    if (data == NULL)
      return;

    if (data[1] != '/' && data[2] != '/') {
      return;
    }

    gst_type_find_suggest (tf, GST_TYPE_FIND_LIKELY, URI_CAPS);
  }
}