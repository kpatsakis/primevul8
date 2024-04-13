gst_mpegts_pmt_stream_new (void)
{
  GstMpegtsPMTStream *stream;

  stream = g_slice_new0 (GstMpegtsPMTStream);

  stream->descriptors = g_ptr_array_new_with_free_func ((GDestroyNotify)
      gst_mpegts_descriptor_free);

  return stream;
}