gst_rtsp_connection_write (GstRTSPConnection * conn, const guint8 * data,
    guint size, GTimeVal * timeout)
{
  guint offset;
  GstClockTime to;
  GstRTSPResult res;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (data != NULL || size == 0, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->output_stream != NULL, GST_RTSP_EINVAL);

  offset = 0;

  to = timeout ? GST_TIMEVAL_TO_TIME (*timeout) : 0;

  g_socket_set_timeout (conn->write_socket, (to + GST_SECOND - 1) / GST_SECOND);
  res =
      write_bytes (conn->output_stream, data, &offset, size, TRUE,
      conn->cancellable);
  g_socket_set_timeout (conn->write_socket, 0);

  return res;
}