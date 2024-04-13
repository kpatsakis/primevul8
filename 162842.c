gst_rtsp_watch_set_send_backlog (GstRTSPWatch * watch,
    gsize bytes, guint messages)
{
  g_return_if_fail (watch != NULL);

  g_mutex_lock (&watch->mutex);
  watch->max_bytes = bytes;
  watch->max_messages = messages;
  if (!IS_BACKLOG_FULL (watch))
    g_cond_signal (&watch->queue_not_full);
  g_mutex_unlock (&watch->mutex);

  GST_DEBUG ("set backlog to bytes %" G_GSIZE_FORMAT ", messages %u",
      bytes, messages);
}