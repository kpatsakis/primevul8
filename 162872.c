gst_rtsp_watch_write_data (GstRTSPWatch * watch, const guint8 * data,
    guint size, guint * id)
{
  GstRTSPSerializedMessage serialized_message;

  memset (&serialized_message, 0, sizeof (serialized_message));
  serialized_message.data = (guint8 *) data;
  serialized_message.data_size = size;

  return gst_rtsp_watch_write_serialized_messages (watch, &serialized_message,
      1, id);
}