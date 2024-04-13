gst_mpegts_pmt_new (void)
{
  GstMpegtsPMT *pmt;

  pmt = g_slice_new0 (GstMpegtsPMT);

  pmt->descriptors = g_ptr_array_new_with_free_func ((GDestroyNotify)
      gst_mpegts_descriptor_free);
  pmt->streams = g_ptr_array_new_with_free_func ((GDestroyNotify)
      _gst_mpegts_pmt_stream_free);

  return pmt;
}