html_type_find (GstTypeFind * tf, gpointer unused)
{
  const gchar *d, *data;

  data = (const gchar *) gst_type_find_peek (tf, 0, 16);
  if (!data)
    return;

  if (!g_ascii_strncasecmp (data, "<!DOCTYPE HTML", 14)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, HTML_CAPS);
  } else if (xml_check_first_element (tf, "html", 4, FALSE)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, HTML_CAPS);
  } else if ((d = memchr (data, '<', 16))) {
    data = (const gchar *) gst_type_find_peek (tf, d - data, 6);
    if (data && g_ascii_strncasecmp (data, "<html>", 6) == 0) {
      gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, HTML_CAPS);
    }
  }
}