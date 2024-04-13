gst_message_new_mpegts_section (GstObject * parent, GstMpegtsSection * section)
{
  GstMessage *msg;
  GstStructure *st;

  st = _mpegts_section_get_structure (section);

  msg = gst_message_new_element (parent, st);

  return msg;
}