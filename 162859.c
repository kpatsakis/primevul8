build_reset (GstRTSPBuilder * builder)
{
  g_free (builder->body_data);
  memset (builder, 0, sizeof (GstRTSPBuilder));
}