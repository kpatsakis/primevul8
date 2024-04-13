_parse_pmt (GstMpegtsSection * section)
{
  GstMpegtsPMT *pmt = NULL;
  guint i = 0, allocated_streams = 8;
  guint8 *data, *end;
  guint program_info_length;
  guint stream_info_length;

  pmt = g_slice_new0 (GstMpegtsPMT);

  data = section->data;
  end = data + section->section_length;

  GST_DEBUG ("Parsing %d Program Map Table", section->subtable_extension);

  /* Assign program number from subtable extension,
     and skip already parsed data */
  pmt->program_number = section->subtable_extension;
  data += 8;

  pmt->pcr_pid = GST_READ_UINT16_BE (data) & 0x1FFF;
  data += 2;

  program_info_length = GST_READ_UINT16_BE (data) & 0x0FFF;
  data += 2;

  /* check that the buffer is large enough to contain at least
   * program_info_length bytes + CRC */
  if (program_info_length && (data + program_info_length + 4 > end)) {
    GST_WARNING ("PID %d invalid program info length %d left %d",
        section->pid, program_info_length, (gint) (end - data));
    goto error;
  }
  pmt->descriptors = gst_mpegts_parse_descriptors (data, program_info_length);
  if (pmt->descriptors == NULL)
    goto error;
  data += program_info_length;

  pmt->streams =
      g_ptr_array_new_full (allocated_streams,
      (GDestroyNotify) _gst_mpegts_pmt_stream_free);

  /* parse entries, cycle until there's space for another entry (at least 5
   * bytes) plus the CRC */
  while (data <= end - 4 - 5) {
    GstMpegtsPMTStream *stream = g_slice_new0 (GstMpegtsPMTStream);

    g_ptr_array_add (pmt->streams, stream);

    stream->stream_type = *data++;
    GST_DEBUG ("[%d] Stream type 0x%02x found", i, stream->stream_type);

    stream->pid = GST_READ_UINT16_BE (data) & 0x1FFF;
    data += 2;

    stream_info_length = GST_READ_UINT16_BE (data) & 0x0FFF;
    data += 2;

    if (data + stream_info_length + 4 > end) {
      GST_WARNING ("PID %d invalid stream info length %d left %d", section->pid,
          stream_info_length, (gint) (end - data));
      goto error;
    }

    stream->descriptors =
        gst_mpegts_parse_descriptors (data, stream_info_length);
    if (stream->descriptors == NULL)
      goto error;
    data += stream_info_length;

    i += 1;
  }

  g_assert (data == end - 4);

  return (gpointer) pmt;

error:
  if (pmt)
    _gst_mpegts_pmt_free (pmt);

  return NULL;
}