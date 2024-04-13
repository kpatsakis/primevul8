gst_rtsp_watch_send_messages (GstRTSPWatch * watch, GstRTSPMessage * messages,
    guint n_messages, guint * id)
{
  GstRTSPSerializedMessage *serialized_messages;
  gint i;

  g_return_val_if_fail (watch != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (messages != NULL || n_messages == 0, GST_RTSP_EINVAL);

  serialized_messages = g_newa (GstRTSPSerializedMessage, n_messages);
  memset (serialized_messages, 0,
      sizeof (GstRTSPSerializedMessage) * n_messages);

  for (i = 0; i < n_messages; i++) {
    if (!serialize_message (watch->conn, &messages[i], &serialized_messages[i]))
      goto error;
  }

  return gst_rtsp_watch_write_serialized_messages (watch, serialized_messages,
      n_messages, id);

error:
  for (i = 0; i < n_messages; i++) {
    gst_rtsp_serialized_message_clear (&serialized_messages[i]);
  }

  return GST_RTSP_EINVAL;
}