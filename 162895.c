gst_rtsp_connection_flush (GstRTSPConnection * conn, gboolean flush)
{
  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);

  if (flush) {
    g_cancellable_cancel (conn->cancellable);
  } else {
    g_object_unref (conn->cancellable);
    conn->cancellable = g_cancellable_new ();
  }

  return GST_RTSP_OK;
}