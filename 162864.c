gst_rtsp_connection_get_tls_database (GstRTSPConnection * conn)
{
  GTlsDatabase *result;

  g_return_val_if_fail (conn != NULL, NULL);

  if ((result = conn->tls_database))
    g_object_ref (result);

  return result;
}