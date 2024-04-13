_gst_mpegts_pmt_stream_copy (GstMpegtsPMTStream * pmt)
{
  GstMpegtsPMTStream *copy;

  copy = g_slice_dup (GstMpegtsPMTStream, pmt);
  copy->descriptors = g_ptr_array_ref (pmt->descriptors);

  return copy;
}