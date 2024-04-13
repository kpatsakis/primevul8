gst_rtsp_watch_get_send_backlog (GstRTSPWatch * watch,
    gsize * bytes, guint * messages)
{
  g_return_if_fail (watch != NULL);

  g_mutex_lock (&watch->mutex);
  if (bytes)
    *bytes = watch->max_bytes;
  if (messages)
    *messages = watch->max_messages;
  g_mutex_unlock (&watch->mutex);
}