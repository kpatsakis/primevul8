gst_rtsp_connection_clear_auth_params (GstRTSPConnection * conn)
{
  g_return_if_fail (conn != NULL);

  if (conn->auth_params != NULL) {
    g_hash_table_destroy (conn->auth_params);
    conn->auth_params = NULL;
  }
}