gst_rtsp_connection_is_tunneled (const GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, FALSE);

  return conn->tunneled;
}