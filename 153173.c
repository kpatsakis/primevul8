tar_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data = gst_type_find_peek (tf, 257, 8);

  /* of course we are not certain, but we don't want other typefind funcs
   * to detect formats of files within the tar archive, e.g. mp3s */
  if (data) {
    if (memcmp (data, OLDGNU_MAGIC, 8) == 0) {  /* sic */
      gst_type_find_suggest (tf, GST_TYPE_FIND_NEARLY_CERTAIN, TAR_CAPS);
    } else if (memcmp (data, NEWGNU_MAGIC, 6) == 0 &&   /* sic */
        g_ascii_isdigit (data[6]) && g_ascii_isdigit (data[7])) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_NEARLY_CERTAIN, TAR_CAPS);
    }
  }
}