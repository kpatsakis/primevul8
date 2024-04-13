gst_rtsp_serialized_message_clear (GstRTSPSerializedMessage * msg)
{
  if (!msg->borrowed) {
    g_free (msg->body_data);
    gst_buffer_replace (&msg->body_buffer, NULL);
  }
  g_free (msg->data);
}