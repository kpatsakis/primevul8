gst_rtsp_connection_send_messages (GstRTSPConnection * conn,
    GstRTSPMessage * messages, guint n_messages, GTimeVal * timeout)
{
  GstClockTime to;
  GstRTSPResult res;
  GstRTSPSerializedMessage *serialized_messages;
  GOutputVector *vectors;
  GstMapInfo *map_infos;
  guint n_vectors, n_memories;
  gint i, j, k;
  gsize bytes_to_write, bytes_written;

  g_return_val_if_fail (conn != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (messages != NULL || n_messages == 0, GST_RTSP_EINVAL);

  serialized_messages = g_newa (GstRTSPSerializedMessage, n_messages);
  memset (serialized_messages, 0,
      sizeof (GstRTSPSerializedMessage) * n_messages);

  for (i = 0, n_vectors = 0, n_memories = 0, bytes_to_write = 0; i < n_messages;
      i++) {
    if (G_UNLIKELY (!serialize_message (conn, &messages[i],
                &serialized_messages[i])))
      goto no_message;

    if (conn->tunneled) {
      gint state = 0, save = 0;
      gchar *base64_buffer, *out_buffer;
      gsize written = 0;
      gsize in_length;

      in_length = serialized_messages[i].data_size;
      if (serialized_messages[i].body_data)
        in_length += serialized_messages[i].body_data_size;
      else if (serialized_messages[i].body_buffer)
        in_length += gst_buffer_get_size (serialized_messages[i].body_buffer);

      in_length = (in_length / 3 + 1) * 4 + 4 + 1;
      base64_buffer = out_buffer = g_malloc0 (in_length);

      written =
          g_base64_encode_step (serialized_messages[i].data_is_data_header ?
          serialized_messages[i].data_header : serialized_messages[i].data,
          serialized_messages[i].data_size, FALSE, out_buffer, &state, &save);
      out_buffer += written;

      if (serialized_messages[i].body_data) {
        written =
            g_base64_encode_step (serialized_messages[i].body_data,
            serialized_messages[i].body_data_size, FALSE, out_buffer, &state,
            &save);
        out_buffer += written;
      } else if (serialized_messages[i].body_buffer) {
        guint j, n = gst_buffer_n_memory (serialized_messages[i].body_buffer);

        for (j = 0; j < n; j++) {
          GstMemory *mem =
              gst_buffer_peek_memory (serialized_messages[i].body_buffer, j);
          GstMapInfo map;

          gst_memory_map (mem, &map, GST_MAP_READ);

          written = g_base64_encode_step (map.data, map.size,
              FALSE, out_buffer, &state, &save);
          out_buffer += written;

          gst_memory_unmap (mem, &map);
        }
      }

      written = g_base64_encode_close (FALSE, out_buffer, &state, &save);
      out_buffer += written;

      gst_rtsp_serialized_message_clear (&serialized_messages[i]);
      memset (&serialized_messages[i], 0, sizeof (serialized_messages[i]));

      serialized_messages[i].data = (guint8 *) base64_buffer;
      serialized_messages[i].data_size = (out_buffer - base64_buffer) + 1;
      n_vectors++;
    } else {
      n_vectors++;
      if (serialized_messages[i].body_data) {
        n_vectors++;
      } else if (serialized_messages[i].body_buffer) {
        n_vectors += gst_buffer_n_memory (serialized_messages[i].body_buffer);
        n_memories += gst_buffer_n_memory (serialized_messages[i].body_buffer);
      }
    }
  }

  vectors = g_newa (GOutputVector, n_vectors);
  map_infos = n_memories ? g_newa (GstMapInfo, n_memories) : NULL;

  for (i = 0, j = 0, k = 0; i < n_messages; i++) {
    vectors[j].buffer = serialized_messages[i].data_is_data_header ?
        serialized_messages[i].data_header : serialized_messages[i].data;
    vectors[j].size = serialized_messages[i].data_size;
    bytes_to_write += vectors[j].size;
    j++;

    if (serialized_messages[i].body_data) {
      vectors[j].buffer = serialized_messages[i].body_data;
      vectors[j].size = serialized_messages[i].body_data_size;
      bytes_to_write += vectors[j].size;
      j++;
    } else if (serialized_messages[i].body_buffer) {
      gint l, n;

      n = gst_buffer_n_memory (serialized_messages[i].body_buffer);
      for (l = 0; l < n; l++) {
        GstMemory *mem =
            gst_buffer_peek_memory (serialized_messages[i].body_buffer, l);

        gst_memory_map (mem, &map_infos[k], GST_MAP_READ);
        vectors[j].buffer = map_infos[k].data;
        vectors[j].size = map_infos[k].size;
        bytes_to_write += vectors[j].size;

        k++;
        j++;
      }
    }
  }

  /* write request: this is synchronous */
  to = timeout ? GST_TIMEVAL_TO_TIME (*timeout) : 0;

  g_socket_set_timeout (conn->write_socket, (to + GST_SECOND - 1) / GST_SECOND);
  res =
      writev_bytes (conn->output_stream, vectors, n_vectors, &bytes_written,
      TRUE, conn->cancellable);
  g_socket_set_timeout (conn->write_socket, 0);

  g_assert (bytes_written == bytes_to_write || res != GST_RTSP_OK);

  /* free everything */
  for (i = 0, k = 0; i < n_messages; i++) {
    if (serialized_messages[i].body_buffer) {
      gint l, n;

      n = gst_buffer_n_memory (serialized_messages[i].body_buffer);
      for (l = 0; l < n; l++) {
        GstMemory *mem =
            gst_buffer_peek_memory (serialized_messages[i].body_buffer, l);

        gst_memory_unmap (mem, &map_infos[k]);
        k++;
      }
    }

    g_free (serialized_messages[i].data);
  }

  return res;

no_message:
  {
    for (i = 0; i < n_messages; i++) {
      gst_rtsp_serialized_message_clear (&serialized_messages[i]);
    }
    g_warning ("Wrong message");
    return GST_RTSP_EINVAL;
  }
}