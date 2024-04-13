gst_rtsp_watch_new (GstRTSPConnection * conn,
    GstRTSPWatchFuncs * funcs, gpointer user_data, GDestroyNotify notify)
{
  GstRTSPWatch *result;

  g_return_val_if_fail (conn != NULL, NULL);
  g_return_val_if_fail (funcs != NULL, NULL);
  g_return_val_if_fail (conn->read_socket != NULL, NULL);
  g_return_val_if_fail (conn->write_socket != NULL, NULL);

  result = (GstRTSPWatch *) g_source_new (&gst_rtsp_source_funcs,
      sizeof (GstRTSPWatch));

  result->conn = conn;
  result->builder.state = STATE_START;

  g_mutex_init (&result->mutex);
  result->messages =
      gst_queue_array_new_for_struct (sizeof (GstRTSPSerializedMessage), 10);
  g_cond_init (&result->queue_not_full);

  gst_rtsp_watch_reset (result);
  result->keep_running = TRUE;
  result->flushing = FALSE;

  result->funcs = *funcs;
  result->user_data = user_data;
  result->notify = notify;

  return result;
}