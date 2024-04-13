_identify_section (guint16 pid, guint8 table_id)
{
  switch (table_id) {
    case GST_MTS_TABLE_ID_PROGRAM_ASSOCIATION:
      if (pid == 0x00)
        return GST_MPEGTS_SECTION_PAT;
      break;
    case GST_MTS_TABLE_ID_CONDITIONAL_ACCESS:
      if (pid == 0x01)
        return GST_MPEGTS_SECTION_CAT;
      break;
    case GST_MTS_TABLE_ID_TS_PROGRAM_MAP:
      return GST_MPEGTS_SECTION_PMT;
    case GST_MTS_TABLE_ID_BOUQUET_ASSOCIATION:
      if (pid == 0x0011)
        return GST_MPEGTS_SECTION_BAT;
      break;
    case GST_MTS_TABLE_ID_NETWORK_INFORMATION_ACTUAL_NETWORK:
    case GST_MTS_TABLE_ID_NETWORK_INFORMATION_OTHER_NETWORK:
      if (pid == 0x0010)
        return GST_MPEGTS_SECTION_NIT;
      break;
    case GST_MTS_TABLE_ID_SERVICE_DESCRIPTION_ACTUAL_TS:
    case GST_MTS_TABLE_ID_SERVICE_DESCRIPTION_OTHER_TS:
      if (pid == 0x0011)
        return GST_MPEGTS_SECTION_SDT;
      break;
    case GST_MTS_TABLE_ID_TIME_DATE:
      if (pid == 0x0014)
        return GST_MPEGTS_SECTION_TDT;
      break;
    case GST_MTS_TABLE_ID_TIME_OFFSET:
      if (pid == 0x0014)
        return GST_MPEGTS_SECTION_TOT;
      break;
    case GST_MTS_TABLE_ID_ATSC_TERRESTRIAL_VIRTUAL_CHANNEL:
      if (pid == 0x1ffb)
        return GST_MPEGTS_SECTION_ATSC_TVCT;
      break;
    case GST_MTS_TABLE_ID_ATSC_CABLE_VIRTUAL_CHANNEL:
      if (pid == 0x1ffb)
        return GST_MPEGTS_SECTION_ATSC_CVCT;
      break;
    case GST_MTS_TABLE_ID_ATSC_MASTER_GUIDE:
      if (pid == 0x1ffb)
        return GST_MPEGTS_SECTION_ATSC_MGT;
      break;
    case GST_MTS_TABLE_ID_ATSC_EVENT_INFORMATION:
      /* FIXME check pids reported on the MGT to confirm expectations */
      return GST_MPEGTS_SECTION_ATSC_EIT;
    case GST_MTS_TABLE_ID_ATSC_CHANNEL_OR_EVENT_EXTENDED_TEXT:
      /* FIXME check pids reported on the MGT to confirm expectations */
      return GST_MPEGTS_SECTION_ATSC_ETT;
      /* FIXME : FILL */
    case GST_MTS_TABLE_ID_ATSC_SYSTEM_TIME:
      if (pid == 0x1ffb)
        return GST_MPEGTS_SECTION_ATSC_STT;
      break;
    default:
      /* Handle ranges */
      if (table_id >= GST_MTS_TABLE_ID_EVENT_INFORMATION_ACTUAL_TS_PRESENT &&
          table_id <= GST_MTS_TABLE_ID_EVENT_INFORMATION_OTHER_TS_SCHEDULE_N) {
        if (pid == 0x0012)
          return GST_MPEGTS_SECTION_EIT;
      }
      return GST_MPEGTS_SECTION_UNKNOWN;
  }
  return GST_MPEGTS_SECTION_UNKNOWN;

}