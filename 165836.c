_gst_mpegts_pmt_stream_free (GstMpegtsPMTStream * pmt)
{
  if (pmt->descriptors)
    g_ptr_array_unref (pmt->descriptors);
  g_slice_free (GstMpegtsPMTStream, pmt);
}