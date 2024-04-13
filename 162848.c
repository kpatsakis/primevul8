gst_rtsp_watch_write_serialized_messages (GstRTSPWatch * watch,
    GstRTSPSerializedMessage * messages, guint n_messages, guint * id)
{
  GstRTSPResult res;
  GMainContext *context = NULL;
  gint i;

  g_return_val_if_fail (watch != NULL, GST_RTSP_EINVAL);
  g_return_val_if_fail (messages != NULL, GST_RTSP_EINVAL);

  g_mutex_lock (&watch->mutex);
  if (watch->flushing)
    goto flushing;

  /* try to send the message synchronously first */
  if (gst_queue_array_get_length (watch->messages) == 0) {
    gint j, k;
    GOutputVector *vectors;
    GstMapInfo *map_infos;
    gsize bytes_to_write, bytes_written;
    guint n_vectors, n_memories, drop_messages;

    for (i = 0, n_vectors = 0, n_memories = 0; i < n_messages; i++) {
      n_vectors++;
      if (messages[i].body_data) {
        n_vectors++;
      } else if (messages[i].body_buffer) {
        n_vectors += gst_buffer_n_memory (messages[i].body_buffer);
        n_memories += gst_buffer_n_memory (messages[i].body_buffer);
      }
    }

    vectors = g_newa (GOutputVector, n_vectors);
    map_infos = n_memories ? g_newa (GstMapInfo, n_memories) : NULL;

    for (i = 0, j = 0, k = 0, bytes_to_write = 0; i < n_messages; i++) {
      vectors[j].buffer = messages[i].data_is_data_header ?
          messages[i].data_header : messages[i].data;
      vectors[j].size = messages[i].data_size;
      bytes_to_write += vectors[j].size;
      j++;

      if (messages[i].body_data) {
        vectors[j].buffer = messages[i].body_data;
        vectors[j].size = messages[i].body_data_size;
        bytes_to_write += vectors[j].size;
        j++;
      } else if (messages[i].body_buffer) {
        gint l, n;

        n = gst_buffer_n_memory (messages[i].body_buffer);
        for (l = 0; l < n; l++) {
          GstMemory *mem = gst_buffer_peek_memory (messages[i].body_buffer, l);

          gst_memory_map (mem, &map_infos[k], GST_MAP_READ);
          vectors[j].buffer = map_infos[k].data;
          vectors[j].size = map_infos[k].size;
          bytes_to_write += vectors[j].size;

          k++;
          j++;
        }
      }
    }

    res =
        writev_bytes (watch->conn->output_stream, vectors, n_vectors,
        &bytes_written, FALSE, watch->conn->cancellable);
    g_assert (bytes_written == bytes_to_write || res != GST_RTSP_OK);

    /* At this point we sent everything we could without blocking or
     * error and updated the offsets inside the message accordingly */

    /* First of all unmap all memories. This simplifies the code below */
    for (k = 0; k < n_memories; k++) {
      gst_memory_unmap (map_infos[k].memory, &map_infos[k]);
    }

    if (res != GST_RTSP_EINTR) {
      /* actual error or done completely */
      if (id != NULL)
        *id = 0;

      /* free everything */
      for (i = 0, k = 0; i < n_messages; i++) {
        gst_rtsp_serialized_message_clear (&messages[i]);
      }

      goto done;
    }

    /* not done, let's skip all messages that were sent already and free them */
    for (i = 0, k = 0, drop_messages = 0; i < n_messages; i++) {
      if (bytes_written >= 0) {
        if (bytes_written >= messages[i].data_size) {
          guint body_size;

          /* all data of this message is sent, check body and otherwise
           * skip the whole message for next time */
          messages[i].data_offset = messages[i].data_size;
          bytes_written -= messages[i].data_size;

          if (messages[i].body_data) {
            body_size = messages[i].body_data_size;

          } else if (messages[i].body_buffer) {
            body_size = gst_buffer_get_size (messages[i].body_buffer);
          } else {
            body_size = 0;
          }

          if (bytes_written >= body_size) {
            /* body written, drop this message */
            messages[i].body_offset = body_size;
            bytes_written -= body_size;
            drop_messages++;

            gst_rtsp_serialized_message_clear (&messages[i]);
          } else {
            messages[i].body_offset = bytes_written;
            bytes_written = 0;
          }
        } else {
          /* Need to continue sending from the data of this message */
          messages[i].data_offset = bytes_written;
          bytes_written = 0;
        }
      }
    }

    g_assert (n_messages > drop_messages);

    messages += drop_messages;
    n_messages -= drop_messages;
  }

  /* check limits */
  if (IS_BACKLOG_FULL (watch))
    goto too_much_backlog;

  for (i = 0; i < n_messages; i++) {
    GstRTSPSerializedMessage local_message;

    /* make a record with the data and id for sending async */
    local_message = messages[i];

    /* copy the body data or take an additional reference to the body buffer
     * we don't own them here */
    if (local_message.body_data) {
      local_message.body_data =
          g_memdup (local_message.body_data, local_message.body_data_size);
    } else if (local_message.body_buffer) {
      gst_buffer_ref (local_message.body_buffer);
    }
    local_message.borrowed = FALSE;

    /* set an id for the very last message */
    if (i == n_messages - 1) {
      do {
        /* make sure rec->id is never 0 */
        local_message.id = ++watch->id;
      } while (G_UNLIKELY (local_message.id == 0));

      if (id != NULL)
        *id = local_message.id;
    } else {
      local_message.id = 0;
    }

    /* add the record to a queue. */
    gst_queue_array_push_tail_struct (watch->messages, &local_message);
    watch->messages_bytes +=
        (local_message.data_size - local_message.data_offset);
    if (local_message.body_data)
      watch->messages_bytes +=
          (local_message.body_data_size - local_message.body_offset);
    else if (local_message.body_buffer)
      watch->messages_bytes +=
          (gst_buffer_get_size (local_message.body_buffer) -
          local_message.body_offset);
  }
  /* each message chunks is one unit */
  watch->messages_count++;

  /* make sure the main context will now also check for writability on the
   * socket */
  context = ((GSource *) watch)->context;
  if (!watch->writesrc) {
    /* remove the read source on the write socket, we will be able to detect
     * errors while writing */
    if (watch->controlsrc) {
      g_source_remove_child_source ((GSource *) watch, watch->controlsrc);
      g_source_unref (watch->controlsrc);
      watch->controlsrc = NULL;
    }

    watch->writesrc =
        g_pollable_output_stream_create_source (G_POLLABLE_OUTPUT_STREAM
        (watch->conn->output_stream), NULL);
    g_source_set_callback (watch->writesrc,
        (GSourceFunc) gst_rtsp_source_dispatch_write, watch, NULL);
    g_source_add_child_source ((GSource *) watch, watch->writesrc);
  }
  res = GST_RTSP_OK;

done:
  g_mutex_unlock (&watch->mutex);

  if (context)
    g_main_context_wakeup (context);

  return res;

  /* ERRORS */
flushing:
  {
    GST_DEBUG ("we are flushing");
    g_mutex_unlock (&watch->mutex);
    for (i = 0; i < n_messages; i++) {
      gst_rtsp_serialized_message_clear (&messages[i]);
    }
    return GST_RTSP_EINTR;
  }
too_much_backlog:
  {
    GST_WARNING ("too much backlog: max_bytes %" G_GSIZE_FORMAT ", current %"
        G_GSIZE_FORMAT ", max_messages %u, current %u", watch->max_bytes,
        watch->messages_bytes, watch->max_messages, watch->messages_count);
    g_mutex_unlock (&watch->mutex);
    for (i = 0; i < n_messages; i++) {
      gst_rtsp_serialized_message_clear (&messages[i]);
    }
    return GST_RTSP_ENOMEM;
  }

  return GST_RTSP_OK;
}