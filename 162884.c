gst_rtsp_watch_unref (GstRTSPWatch * watch)
{
  g_return_if_fail (watch != NULL);

  g_source_unref ((GSource *) watch);
}