_packetize_common_section (GstMpegtsSection * section, gsize length)
{
  guint8 *data;

  section->section_length = length;
  data = section->data = g_malloc (length);

  /* table_id                         - 8 bit uimsbf */
  *data++ = section->table_id;

  /* section_syntax_indicator         - 1  bit
     reserved                         - 3  bit
     section_length                   - 12 bit uimsbf */
  switch (section->section_type) {
    case GST_MPEGTS_SECTION_PAT:
    case GST_MPEGTS_SECTION_PMT:
    case GST_MPEGTS_SECTION_CAT:
    case GST_MPEGTS_SECTION_TSDT:
      /* Tables from ISO/IEC 13818-1 has a '0' bit
       * after the section_syntax_indicator */
      GST_WRITE_UINT16_BE (data, (section->section_length - 3) | 0x3000);
      break;
    default:
      GST_WRITE_UINT16_BE (data, (section->section_length - 3) | 0x7000);
  }

  /* short sections do not contain any further fields */
  if (section->short_section)
    return;

  /* Set section_syntax_indicator bit since we do not have a short section */
  *data |= 0x80;
  data += 2;

  /* subtable_extension               - 16 bit uimsbf */
  GST_WRITE_UINT16_BE (data, section->subtable_extension);
  data += 2;

  /* reserved                         - 2  bit
     version_number                   - 5  bit uimsbf
     current_next_indicator           - 1  bit */
  *data++ = 0xC0 |
      ((section->version_number & 0x1F) << 1) |
      (section->current_next_indicator & 0x01);

  /* section_number                   - 8  bit uimsbf */
  *data++ = section->section_number;
  /* last_section_number              - 8  bit uimsbf */
  *data++ = section->last_section_number;
}