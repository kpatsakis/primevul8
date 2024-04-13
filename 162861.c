gst_rtsp_source_prepare (GSource * source, gint * timeout)
{
  GstRTSPWatch *watch = (GstRTSPWatch *) source;

  if (watch->conn->initial_buffer != NULL)
    return TRUE;

  *timeout = (watch->conn->timeout * 1000);

  return FALSE;
}