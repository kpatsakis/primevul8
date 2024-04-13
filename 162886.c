gen_tunnel_reply (GstRTSPConnection * conn, GstRTSPStatusCode code,
    const GstRTSPMessage * request)
{
  GstRTSPMessage *msg;
  GstRTSPResult res;

  if (gst_rtsp_status_as_text (code) == NULL)
    code = GST_RTSP_STS_INTERNAL_SERVER_ERROR;

  GST_RTSP_CHECK (gst_rtsp_message_new_response (&msg, code, NULL, request),
      no_message);

  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_SERVER,
      "GStreamer RTSP Server");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CONNECTION, "close");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CACHE_CONTROL, "no-store");
  gst_rtsp_message_add_header (msg, GST_RTSP_HDR_PRAGMA, "no-cache");

  if (code == GST_RTSP_STS_OK) {
    /* add the local ip address to the tunnel reply, this is where the client
     * should send the POST request to */
    if (conn->local_ip)
      gst_rtsp_message_add_header (msg, GST_RTSP_HDR_X_SERVER_IP_ADDRESS,
          conn->local_ip);
    gst_rtsp_message_add_header (msg, GST_RTSP_HDR_CONTENT_TYPE,
        "application/x-rtsp-tunnelled");
  }

  return msg;

  /* ERRORS */
no_message:
  {
    return NULL;
  }
}