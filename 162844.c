gst_rtsp_connection_set_auth (GstRTSPConnection * conn,
    GstRTSPAuthMethod method, const gchar * user, const gchar * pass)
{
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  if (method == GST_RTSP_AUTH_DIGEST && ((user == NULL || pass == NULL)
          || g_strrstr (user, ":") != NULL))
    return GST_RTSP_EINVAL;

  /* Make sure the username and passwd are being set for authentication */
  if (method == GST_RTSP_AUTH_NONE && (user == NULL || pass == NULL))
    return GST_RTSP_EINVAL;

  /* ":" chars are not allowed in usernames for basic auth */
  if (method == GST_RTSP_AUTH_BASIC && g_strrstr (user, ":") != NULL)
    return GST_RTSP_EINVAL;

  g_free (conn->username);
  g_free (conn->passwd);

  conn->auth_method = method;
  conn->username = g_strdup (user);
  conn->passwd = g_strdup (pass);

  return GST_RTSP_OK;
}