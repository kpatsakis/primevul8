gst_rtsp_connection_get_tunnelid (const GstRTSPConnection * conn)
{
  g_return_val_if_fail (conn != NULL, NULL);

  if (!conn->tunneled)
    return NULL;

  return conn->tunnelid;
}