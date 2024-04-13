gst_rtsp_connection_set_qos_dscp (GstRTSPConnection * conn, guint qos_dscp)
{
  GstRTSPResult res;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->read_socket != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (conn->write_socket != NULL, GST_RTSP_EINVAL);

  res = set_qos_dscp (conn->socket0, qos_dscp);
  if (res == GST_RTSP_OK)
    res = set_qos_dscp (conn->socket1, qos_dscp);

  return res;
}