_gst_mpegts_pmt_free (GstMpegtsPMT * pmt)
{
  if (pmt->descriptors)
    g_ptr_array_unref (pmt->descriptors);
  if (pmt->streams)
    g_ptr_array_unref (pmt->streams);
  g_slice_free (GstMpegtsPMT, pmt);
}