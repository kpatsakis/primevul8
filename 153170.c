dash_mpd_type_find (GstTypeFind * tf, gpointer unused)
{
  if (xml_check_first_element (tf, "MPD", 3, FALSE) ||
      xml_check_first_element (tf, "mpd", 3, FALSE)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, DASH_CAPS);
  }
}