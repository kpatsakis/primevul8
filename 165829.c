_gst_mpegts_pmt_copy (GstMpegtsPMT * pmt)
{
  GstMpegtsPMT *copy;

  copy = g_slice_dup (GstMpegtsPMT, pmt);
  if (pmt->descriptors)
    copy->descriptors = g_ptr_array_ref (pmt->descriptors);
  copy->streams = g_ptr_array_ref (pmt->streams);

  return copy;
}