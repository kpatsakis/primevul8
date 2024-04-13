gst_rtsp_connection_set_auth_param (GstRTSPConnection * conn,
    const gchar * param, const gchar * value)
{
  g_return_if_fail (conn != NULL);
  g_return_if_fail (param != NULL);

  if (conn->auth_params == NULL) {
    conn->auth_params =
        g_hash_table_new_full (str_case_hash, str_case_equal, g_free, g_free);
  }
  g_hash_table_insert (conn->auth_params, g_strdup (param), g_strdup (value));
}