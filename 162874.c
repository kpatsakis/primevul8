gst_rtsp_source_dispatch_write (GPollableOutputStream * stream,
    GstRTSPWatch * watch)
{
  GstRTSPResult res = GST_RTSP_ERROR;
  GstRTSPConnection *conn = watch->conn;

  /* if this connection was already closed, stop now */
  if (G_POLLABLE_OUTPUT_STREAM (conn->output_stream) != stream ||
      !watch->messages)
    goto eof;

  g_mutex_lock (&watch->mutex);
  do {
    guint n_messages = gst_queue_array_get_length (watch->messages);
    GOutputVector *vectors;
    GstMapInfo *map_infos;
    guint *ids;
    gsize bytes_to_write, bytes_written;
    guint n_vectors, n_memories, n_ids, drop_messages;
    gint i, j, l, n_mmap;
    GstRTSPSerializedMessage *msg;

    /* if this connection was already closed, stop now */
    if (G_POLLABLE_OUTPUT_STREAM (conn->output_stream) != stream ||
        !watch->messages) {
      g_mutex_unlock (&watch->mutex);
      goto eof;
    }

    if (n_messages == 0) {
      if (watch->writesrc) {
        if (!g_source_is_destroyed ((GSource *) watch))
          g_source_remove_child_source ((GSource *) watch, watch->writesrc);
        g_source_unref (watch->writesrc);
        watch->writesrc = NULL;
        /* we create and add the write source again when we actually have
         * something to write */

        /* since write source is now removed we add read source on the write
         * socket instead to be able to detect when client closes get channel
         * in tunneled mode */
        if (watch->conn->control_stream) {
          watch->controlsrc =
              g_pollable_input_stream_create_source (G_POLLABLE_INPUT_STREAM
              (watch->conn->control_stream), NULL);
          g_source_set_callback (watch->controlsrc,
              (GSourceFunc) gst_rtsp_source_dispatch_read_get_channel, watch,
              NULL);
          g_source_add_child_source ((GSource *) watch, watch->controlsrc);
        } else {
          watch->controlsrc = NULL;
        }
      }
      break;
    }

    for (i = 0, n_vectors = 0, n_memories = 0, n_ids = 0; i < n_messages; i++) {
      msg = gst_queue_array_peek_nth_struct (watch->messages, i);
      if (msg->id != 0)
        n_ids++;

      if (msg->data_offset < msg->data_size)
        n_vectors++;

      if (msg->body_data && msg->body_offset < msg->body_data_size) {
        n_vectors++;
      } else if (msg->body_buffer) {
        guint m, n;
        guint offset = 0;

        n = gst_buffer_n_memory (msg->body_buffer);
        for (m = 0; m < n; m++) {
          GstMemory *mem = gst_buffer_peek_memory (msg->body_buffer, m);

          /* Skip all memories we already wrote */
          if (offset + mem->size < msg->body_offset) {
            offset += mem->size;
            continue;
          }
          offset += mem->size;

          n_memories++;
          n_vectors++;
        }
      }
    }

    vectors = g_newa (GOutputVector, n_vectors);
    map_infos = n_memories ? g_newa (GstMapInfo, n_memories) : NULL;
    ids = n_ids ? g_newa (guint, n_ids + 1) : NULL;
    if (ids)
      memset (ids, 0, sizeof (guint) * (n_ids + 1));

    for (i = 0, j = 0, n_mmap = 0, l = 0, bytes_to_write = 0; i < n_messages;
        i++) {
      msg = gst_queue_array_peek_nth_struct (watch->messages, i);

      if (msg->data_offset < msg->data_size) {
        vectors[j].buffer = (msg->data_is_data_header ?
            msg->data_header : msg->data) + msg->data_offset;
        vectors[j].size = msg->data_size - msg->data_offset;
        bytes_to_write += vectors[j].size;
        j++;
      }

      if (msg->body_data) {
        if (msg->body_offset < msg->body_data_size) {
          vectors[j].buffer = msg->body_data + msg->body_offset;
          vectors[j].size = msg->body_data_size - msg->body_offset;
          bytes_to_write += vectors[j].size;
          j++;
        }
      } else if (msg->body_buffer) {
        guint m, n;
        guint offset = 0;
        n = gst_buffer_n_memory (msg->body_buffer);
        for (m = 0; m < n; m++) {
          GstMemory *mem = gst_buffer_peek_memory (msg->body_buffer, m);
          guint off;

          /* Skip all memories we already wrote */
          if (offset + mem->size <= msg->body_offset) {
            offset += mem->size;
            continue;
          }

          if (offset < msg->body_offset)
            off = msg->body_offset - offset;
          else
            off = 0;

          offset += mem->size;

          g_assert (off < mem->size);

          gst_memory_map (mem, &map_infos[n_mmap], GST_MAP_READ);
          vectors[j].buffer = map_infos[n_mmap].data + off;
          vectors[j].size = map_infos[n_mmap].size - off;
          bytes_to_write += vectors[j].size;

          n_mmap++;
          j++;
        }
      }
    }

    res =
        writev_bytes (watch->conn->output_stream, vectors, n_vectors,
        &bytes_written, FALSE, watch->conn->cancellable);
    g_assert (bytes_written == bytes_to_write || res != GST_RTSP_OK);

    /* First unmap all memories here, this simplifies the code below
     * as we don't have to skip all memories that were already written
     * before */
    for (i = 0; i < n_mmap; i++) {
      gst_memory_unmap (map_infos[i].memory, &map_infos[i]);
    }

    if (bytes_written == bytes_to_write) {
      /* fast path, just unmap all memories, free memory, drop all messages and notify them */
      l = 0;
      while ((msg = gst_queue_array_pop_head_struct (watch->messages))) {
        if (msg->id) {
          ids[l] = msg->id;
          l++;
        }

        gst_rtsp_serialized_message_clear (msg);
      }

      g_assert (watch->messages_bytes >= bytes_written);
      watch->messages_bytes -= bytes_written;
    } else if (bytes_written > 0) {
      /* not done, let's skip all messages that were sent already and free them */
      for (i = 0, drop_messages = 0; i < n_messages; i++) {
        msg = gst_queue_array_peek_nth_struct (watch->messages, i);

        if (bytes_written >= 0) {
          if (bytes_written >= msg->data_size - msg->data_offset) {
            guint body_size;

            /* all data of this message is sent, check body and otherwise
             * skip the whole message for next time */
            bytes_written -= (msg->data_size - msg->data_offset);
            msg->data_offset = msg->data_size;

            if (msg->body_data) {
              body_size = msg->body_data_size;
            } else if (msg->body_buffer) {
              body_size = gst_buffer_get_size (msg->body_buffer);
            } else {
              body_size = 0;
            }

            if (bytes_written + msg->body_offset >= body_size) {
              /* body written, drop this message */
              bytes_written -= body_size - msg->body_offset;
              msg->body_offset = body_size;
              drop_messages++;

              if (msg->id) {
                ids[l] = msg->id;
                l++;
              }

              gst_rtsp_serialized_message_clear (msg);
            } else {
              msg->body_offset += bytes_written;
              bytes_written = 0;
            }
          } else {
            /* Need to continue sending from the data of this message */
            msg->data_offset = bytes_written;
            bytes_written = 0;
          }
        }
      }

      while (drop_messages > 0) {
        msg = gst_queue_array_pop_head_struct (watch->messages);
        g_assert (msg);
        drop_messages--;
      }

      g_assert (watch->messages_bytes >= bytes_written);
      watch->messages_bytes -= bytes_written;
    }

    if (!IS_BACKLOG_FULL (watch))
      g_cond_signal (&watch->queue_not_full);
    g_mutex_unlock (&watch->mutex);

    /* notify all messages that were successfully written */
    if (ids) {
      while (*ids) {
        /* only decrease the counter for messages that have an id. Only
         * the last message of a messages chunk is counted */
        watch->messages_count--;

        if (watch->funcs.message_sent)
          watch->funcs.message_sent (watch, *ids, watch->user_data);
        ids++;
      }
    }

    if (res == GST_RTSP_EINTR) {
      goto write_blocked;
    } else if (G_UNLIKELY (res != GST_RTSP_OK)) {
      goto write_error;
    }
    g_mutex_lock (&watch->mutex);
  } while (TRUE);
  g_mutex_unlock (&watch->mutex);

write_blocked:
  return TRUE;

  /* ERRORS */
eof:
  {
    return FALSE;
  }
write_error:
  {
    if (watch->funcs.error_full) {
      guint i, n_messages;

      n_messages = gst_queue_array_get_length (watch->messages);
      for (i = 0; i < n_messages; i++) {
        GstRTSPSerializedMessage *msg =
            gst_queue_array_peek_nth_struct (watch->messages, i);
        if (msg->id)
          watch->funcs.error_full (watch, res, NULL, msg->id, watch->user_data);
      }
    } else if (watch->funcs.error) {
      watch->funcs.error (watch, res, watch->user_data);
    }

    return FALSE;
  }
}