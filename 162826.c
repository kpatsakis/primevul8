gst_rtsp_connection_set_remember_session_id (GstRTSPConnection * conn,
    gboolean remember)
{
  conn->remember_session_id = remember;
  if (!remember)
    conn->session_id[0] = '\0';
}