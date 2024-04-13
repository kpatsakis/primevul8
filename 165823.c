_mpegts_section_get_event (GstMpegtsSection * section)
{
  GstStructure *structure;
  GstEvent *event;

  structure = _mpegts_section_get_structure (section);

  event = gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM, structure);

  return event;
}