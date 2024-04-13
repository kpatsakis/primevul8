mss_manifest_type_find (GstTypeFind * tf, gpointer unused)
{
  gunichar2 utf16_ne[512];
  const guint8 *data;
  guint data_endianness = 0;
  glong n_read = 0, size = 0;
  guint length;
  gchar *utf8;
  gboolean utf8_bom_detected = FALSE;

  if (xml_check_first_element (tf, "SmoothStreamingMedia", 20, TRUE)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MSS_MANIFEST_CAPS);
    return;
  }

  length = gst_type_find_get_length (tf);

  /* try detecting the charset */
  data = gst_type_find_peek (tf, 0, 3);

  if (data == NULL)
    return;

  /* look for a possible BOM */
  if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
    utf8_bom_detected = TRUE;
  else if (data[0] == 0xFF && data[1] == 0xFE)
    data_endianness = G_LITTLE_ENDIAN;
  else if (data[0] == 0xFE && data[1] == 0xFF)
    data_endianness = G_BIG_ENDIAN;
  else
    return;

  /* try a default that should be enough */
  if (length == 0)
    length = 512;
  else if (length < 64)
    return;
  else                          /* the first few bytes should be enough */
    length = MIN (1024, length);

  data = gst_type_find_peek (tf, 0, length);

  if (data == NULL)
    return;

  /* skip the BOM */
  data += 2;
  length -= 2;

  if (utf8_bom_detected) {
    /* skip last byte of the BOM */
    data++;
    length--;

    if (xml_check_first_element_from_data (data, length,
            "SmoothStreamingMedia", 20, TRUE))
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MSS_MANIFEST_CAPS);
  } else {
    length = GST_ROUND_DOWN_2 (length);

    /* convert to native endian UTF-16 */
    mss_manifest_load_utf16 (utf16_ne, data, length, data_endianness);

    /* and now convert to UTF-8 */
    utf8 = g_utf16_to_utf8 (utf16_ne, length / 2, &n_read, &size, NULL);
    if (utf8 != NULL && n_read > 0) {
      if (xml_check_first_element_from_data ((const guint8 *) utf8, size,
              "SmoothStreamingMedia", 20, TRUE))
        gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, MSS_MANIFEST_CAPS);
    }
    g_free (utf8);
  }
}