gst_rtsp_connection_set_accept_certificate_func (GstRTSPConnection * conn,
    GstRTSPConnectionAcceptCertificateFunc func,
    gpointer user_data, GDestroyNotify destroy_notify)
{
  if (conn->accept_certificate_destroy_notify)
    conn->
        accept_certificate_destroy_notify (conn->accept_certificate_user_data);
  conn->accept_certificate_func = func;
  conn->accept_certificate_user_data = user_data;
  conn->accept_certificate_destroy_notify = destroy_notify;
}