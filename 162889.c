gst_rtsp_watch_attach (GstRTSPWatch * watch, GMainContext * context)
{
  g_return_val_if_fail (watch != NULL, 0);

  return g_source_attach ((GSource *) watch, context);
}