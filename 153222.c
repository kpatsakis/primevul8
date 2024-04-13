smil_type_find (GstTypeFind * tf, gpointer unused)
{
  if (xml_check_first_element (tf, "smil", 4, FALSE)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, SMIL_CAPS);
  }
}