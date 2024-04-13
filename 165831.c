_packetize_pat (GstMpegtsSection * section)
{
  GPtrArray *programs;
  guint8 *data;
  gsize length;
  guint i;

  programs = gst_mpegts_section_get_pat (section);

  if (programs == NULL)
    return FALSE;

  /* 8 byte common section fields
     4 byte CRC */
  length = 12;

  /* 2 byte program number
     2 byte program/network PID */
  length += programs->len * 4;

  _packetize_common_section (section, length);
  data = section->data + 8;

  for (i = 0; i < programs->len; i++) {
    GstMpegtsPatProgram *program;

    program = g_ptr_array_index (programs, i);

    /* program_number       - 16 bit uimsbf */
    GST_WRITE_UINT16_BE (data, program->program_number);
    data += 2;

    /* reserved             - 3  bit
       program/network_PID  - 13 uimsbf */
    GST_WRITE_UINT16_BE (data, program->network_or_program_map_PID | 0xE000);
    data += 2;
  }

  g_ptr_array_unref (programs);

  return TRUE;
}