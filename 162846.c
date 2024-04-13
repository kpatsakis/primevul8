gst_rtsp_connection_send (GstRTSPConnection * conn, GstRTSPMessage * message,
    GTimeVal * timeout)
{
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (message != NULL, GST_RTSP_EINVAL);

  return gst_rtsp_connection_send_messages (conn, message, 1, timeout);
}