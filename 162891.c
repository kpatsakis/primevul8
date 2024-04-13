gst_rtsp_source_dispatch (GSource * source, GSourceFunc callback G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{
  GstRTSPWatch *watch = (GstRTSPWatch *) source;
  GstRTSPConnection *conn = watch->conn;

  if (conn->initial_buffer != NULL) {
    gst_rtsp_source_dispatch_read (G_POLLABLE_INPUT_STREAM (conn->input_stream),
        watch);
  }
  return watch->keep_running;
}