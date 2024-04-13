gst_rtsp_connection_get_tls_interaction (GstRTSPConnection * conn)
{
  GTlsInteraction *result;

  g_return_val_if_fail (conn != NULL, NULL);

  if ((result = conn->tls_interaction))
    g_object_ref (result);

  return result;
}