q3gp_type_find (GstTypeFind * tf, gpointer unused)
{
  const gchar *profile;
  guint32 ftyp_size = 0;
  gint offset = 0;
  const guint8 *data = NULL;

  if ((data = gst_type_find_peek (tf, 0, 12)) == NULL) {
    return;
  }

  data += 4;
  if (memcmp (data, "ftyp", 4) != 0) {
    return;
  }

  /* check major brand */
  data += 4;
  if ((profile = q3gp_type_find_get_profile (data))) {
    gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM,
        "application/x-3gp", "profile", G_TYPE_STRING, profile, NULL);
    return;
  }

  /* check compatible brands */
  if ((data = gst_type_find_peek (tf, 0, 4)) != NULL) {
    ftyp_size = GST_READ_UINT32_BE (data);
  }
  for (offset = 16; offset < ftyp_size; offset += 4) {
    if ((data = gst_type_find_peek (tf, offset, 3)) == NULL) {
      break;
    }
    if ((profile = q3gp_type_find_get_profile (data))) {
      gst_type_find_suggest_simple (tf, GST_TYPE_FIND_MAXIMUM,
          "application/x-3gp", "profile", G_TYPE_STRING, profile, NULL);
      return;
    }
  }

  return;

}