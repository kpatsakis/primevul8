xdgmime_typefind (GstTypeFind * find, gpointer user_data)
{
  gchar *mimetype;
  gsize length = 16384;
  guint64 tf_length;
  const guint8 *data;
  gchar *tmp;

  if ((tf_length = gst_type_find_get_length (find)) > 0)
    length = MIN (length, tf_length);

  if ((data = gst_type_find_peek (find, 0, length)) == NULL)
    return;

  tmp = g_content_type_guess (NULL, data, length, NULL);
  if (tmp == NULL || g_content_type_is_unknown (tmp)) {
    g_free (tmp);
    return;
  }

  mimetype = g_content_type_get_mime_type (tmp);
  g_free (tmp);

  if (mimetype == NULL)
    return;

  GST_DEBUG ("Got mimetype '%s'", mimetype);

  /* Ignore audio/video types:
   *  - our own typefinders in -base are likely to be better at this
   *    (and if they're not, we really want to fix them, that's why we don't
   *    report xdg-detected audio/video types at all, not even with a low
   *    probability)
   *  - we want to detect GStreamer media types and not MIME types
   *  - the purpose of this xdg mime finder is mainly to prevent false
   *    positives of non-media formats, not to typefind audio/video formats */
  if (g_str_has_prefix (mimetype, "audio/") ||
      g_str_has_prefix (mimetype, "video/")) {
    GST_LOG ("Ignoring audio/video mime type");
    g_free (mimetype);
    return;
  }

  /* Again, we mainly want the xdg typefinding to prevent false-positives on
   * non-media formats, so suggest the type with a probability that trumps
   * uncertain results of our typefinders, but not more than that. */
  GST_LOG ("Suggesting '%s' with probability POSSIBLE", mimetype);
  gst_type_find_suggest_simple (find, GST_TYPE_FIND_POSSIBLE, mimetype, NULL);
  g_free (mimetype);
}