gst_message_parse_mpegts_section (GstMessage * message)
{
  const GstStructure *st;
  GstMpegtsSection *section;

  if (message->type != GST_MESSAGE_ELEMENT)
    return NULL;

  st = gst_message_get_structure (message);
  /* FIXME : Add checks against know section names */
  if (!gst_structure_id_get (st, QUARK_SECTION, GST_TYPE_MPEGTS_SECTION,
          &section, NULL))
    return NULL;

  return section;
}