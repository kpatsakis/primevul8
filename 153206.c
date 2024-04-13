ttml_xml_type_find (GstTypeFind * tf, gpointer unused)
{
  if (xml_check_first_element (tf, "tt", 2, FALSE)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MAXIMUM, TTML_XML_CAPS);
  }
}