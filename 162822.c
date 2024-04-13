serialize_message (GstRTSPConnection * conn, GstRTSPMessage * message,
    GstRTSPSerializedMessage * serialized_message)
{
  GString *str = NULL;

  memset (serialized_message, 0, sizeof (*serialized_message));

  /* Initially we borrow the body_data / body_buffer fields from
   * the message */
  serialized_message->borrowed = TRUE;

  switch (message->type) {
    case GST_RTSP_MESSAGE_REQUEST:
      str = g_string_new ("");

      /* create request string, add CSeq */
      g_string_append_printf (str, "%s %s RTSP/%s\r\n"
          "CSeq: %d\r\n",
          gst_rtsp_method_as_text (message->type_data.request.method),
          message->type_data.request.uri,
          gst_rtsp_version_as_text (message->type_data.request.version),
          conn->cseq++);
      /* add session id if we have one */
      if (conn->session_id[0] != '\0') {
        gst_rtsp_message_remove_header (message, GST_RTSP_HDR_SESSION, -1);
        gst_rtsp_message_add_header (message, GST_RTSP_HDR_SESSION,
            conn->session_id);
      }
      /* add any authentication headers */
      add_auth_header (conn, message);
      break;
    case GST_RTSP_MESSAGE_RESPONSE:
      str = g_string_new ("");

      /* create response string */
      g_string_append_printf (str, "RTSP/%s %d %s\r\n",
          gst_rtsp_version_as_text (message->type_data.response.version),
          message->type_data.response.code, message->type_data.response.reason);
      break;
    case GST_RTSP_MESSAGE_HTTP_REQUEST:
      str = g_string_new ("");

      /* create request string */
      g_string_append_printf (str, "%s %s HTTP/%s\r\n",
          gst_rtsp_method_as_text (message->type_data.request.method),
          message->type_data.request.uri,
          gst_rtsp_version_as_text (message->type_data.request.version));
      /* add any authentication headers */
      add_auth_header (conn, message);
      break;
    case GST_RTSP_MESSAGE_HTTP_RESPONSE:
      str = g_string_new ("");

      /* create response string */
      g_string_append_printf (str, "HTTP/%s %d %s\r\n",
          gst_rtsp_version_as_text (message->type_data.request.version),
          message->type_data.response.code, message->type_data.response.reason);
      break;
    case GST_RTSP_MESSAGE_DATA:
    {
      guint8 *data_header = serialized_message->data_header;

      /* prepare data header */
      data_header[0] = '$';
      data_header[1] = message->type_data.data.channel;
      data_header[2] = (message->body_size >> 8) & 0xff;
      data_header[3] = message->body_size & 0xff;

      /* create serialized message with header and data */
      serialized_message->data_is_data_header = TRUE;
      serialized_message->data_size = 4;

      if (message->body) {
        serialized_message->body_data = message->body;
        serialized_message->body_data_size = message->body_size;
      } else {
        g_assert (message->body_buffer != NULL);
        serialized_message->body_buffer = message->body_buffer;
      }
      break;
    }
    default:
      g_string_free (str, TRUE);
      g_return_val_if_reached (FALSE);
      break;
  }

  /* append headers and body */
  if (message->type != GST_RTSP_MESSAGE_DATA) {
    gchar date_string[100];

    g_assert (str != NULL);

    gen_date_string (date_string, sizeof (date_string));

    /* add date header */
    gst_rtsp_message_remove_header (message, GST_RTSP_HDR_DATE, -1);
    gst_rtsp_message_add_header (message, GST_RTSP_HDR_DATE, date_string);

    /* append headers */
    gst_rtsp_message_append_headers (message, str);

    /* append Content-Length and body if needed */
    if (message->body_size > 0) {
      gchar *len;

      len = g_strdup_printf ("%d", message->body_size);
      g_string_append_printf (str, "%s: %s\r\n",
          gst_rtsp_header_as_text (GST_RTSP_HDR_CONTENT_LENGTH), len);
      g_free (len);
      /* header ends here */
      g_string_append (str, "\r\n");

      if (message->body) {
        serialized_message->body_data = message->body;
        serialized_message->body_data_size = message->body_size;
      } else {
        g_assert (message->body_buffer != NULL);
        serialized_message->body_buffer = message->body_buffer;
      }
    } else {
      /* just end headers */
      g_string_append (str, "\r\n");
    }

    serialized_message->data_size = str->len;
    serialized_message->data = (guint8 *) g_string_free (str, FALSE);
  }

  return TRUE;
}