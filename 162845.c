gst_rtsp_watch_set_flushing (GstRTSPWatch * watch, gboolean flushing)
{
  g_return_if_fail (watch != NULL);

  g_mutex_lock (&watch->mutex);
  watch->flushing = flushing;
  g_cond_signal (&watch->queue_not_full);
  if (flushing) {
    GstRTSPSerializedMessage *msg;

    while ((msg = gst_queue_array_pop_head_struct (watch->messages))) {
      gst_rtsp_serialized_message_clear (msg);
    }
  }
  g_mutex_unlock (&watch->mutex);
}