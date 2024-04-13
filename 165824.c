gst_mpegts_section_send_event (GstMpegtsSection * section, GstElement * element)
{
  GstEvent *event;

  g_return_val_if_fail (section != NULL, FALSE);
  g_return_val_if_fail (element != NULL, FALSE);

  event = _mpegts_section_get_event (section);

  if (!gst_element_send_event (element, event)) {
    gst_event_unref (event);
    return FALSE;
  }

  return TRUE;
}