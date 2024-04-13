gst_rtsp_watch_send_message (GstRTSPWatch * watch, GstRTSPMessage * message,
    guint * id)
{
  g_return_val_if_fail (watch != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (message != NULL, GST_RTSP_EINVAL);

  return gst_rtsp_watch_send_messages (watch, message, 1, id);
}