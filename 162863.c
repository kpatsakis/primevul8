gst_rtsp_connection_connect (GstRTSPConnection * conn, GTimeVal * timeout)
{
  GstRTSPResult result;
  GstRTSPMessage response;

  memset (&response, 0, sizeof (response));
  gst_rtsp_message_init (&response);

  result = gst_rtsp_connection_connect_with_response (conn, timeout, &response);

  gst_rtsp_message_unset (&response);

  return result;
}