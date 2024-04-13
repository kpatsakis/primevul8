gst_rtsp_connection_read (GstRTSPConnection * conn, guint8 * data, guint size,
    GTimeVal * timeout)
{
  guint offset;
  GstClockTime to;
  GstRTSPResult res;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (data != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->read_socket != NULL, GST_RTSP_EINVAL);

  if (G_UNLIKELY (size == 0))
    return GST_RTSP_OK;

  offset = 0;

  /* configure timeout if any */
  to = timeout ? GST_TIMEVAL_TO_TIME (*timeout) : 0;

  g_socket_set_timeout (conn->read_socket, (to + GST_SECOND - 1) / GST_SECOND);
  res = read_bytes (conn, data, &offset, size, TRUE);
  g_socket_set_timeout (conn->read_socket, 0);

  return res;
}