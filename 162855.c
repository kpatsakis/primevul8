gst_rtsp_source_finalize (GSource * source)
{
  GstRTSPWatch *watch = (GstRTSPWatch *) source;
  GstRTSPSerializedMessage *msg;

  if (watch->notify)
    watch->notify (watch->user_data);

  build_reset (&watch->builder);
  gst_rtsp_message_unset (&watch->message);

  while ((msg = gst_queue_array_pop_head_struct (watch->messages))) {
    gst_rtsp_serialized_message_clear (msg);
  }
  gst_queue_array_free (watch->messages);
  watch->messages = NULL;
  watch->messages_bytes = 0;
  watch->messages_count = 0;

  g_cond_clear (&watch->queue_not_full);

  if (watch->readsrc)
    g_source_unref (watch->readsrc);
  if (watch->writesrc)
    g_source_unref (watch->writesrc);
  if (watch->controlsrc)
    g_source_unref (watch->controlsrc);

  g_mutex_clear (&watch->mutex);
}