xml_type_find (GstTypeFind * tf, gpointer unused)
{
  if (xml_check_first_element (tf, "", 0, TRUE)) {
    gst_type_find_suggest (tf, GST_TYPE_FIND_MINIMUM, GENERIC_XML_CAPS);
  }
}