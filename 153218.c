mpeg_sys_type_find (GstTypeFind * tf, gpointer unused)
{
  const guint8 *data, *data0, *first_sync, *end;
  gint mpegversion = 0;
  guint pack_headers = 0;
  guint pes_headers = 0;
  guint pack_size;
  guint since_last_sync = 0;
  guint32 sync_word = 0xffffffff;
  guint potential_headers = 0;

  G_STMT_START {
    gint len;

    len = MPEG2_MAX_PROBE_LENGTH;

    while (len >= 16) {
      data = gst_type_find_peek (tf, 0, 5 + len);
      if (data != NULL)
        break;
      len = len / 2;
    }

    if (!data)
      return;

    end = data + len;
  }
  G_STMT_END;

  data0 = data;
  first_sync = NULL;

  while (data < end) {
    sync_word <<= 8;
    if (sync_word == 0x00000100) {
      /* Found potential sync word */
      if (first_sync == NULL)
        first_sync = data - 3;

      if (since_last_sync > 4) {
        /* If more than 4 bytes since the last sync word, reset our counters,
         * as we're only interested in counting contiguous packets */
        pes_headers = pack_headers = 0;
      }
      pack_size = 0;

      potential_headers++;
      if (IS_MPEG_PACK_CODE (data[0])) {
        if ((data[1] & 0xC0) == 0x40) {
          /* MPEG-2 */
          mpegversion = 2;
        } else if ((data[1] & 0xF0) == 0x20) {
          mpegversion = 1;
        }
        if (mpegversion != 0 &&
            mpeg_sys_is_valid_pack (tf, data - 3, end - data + 3, &pack_size)) {
          pack_headers++;
        }
      } else if (IS_MPEG_PES_CODE (data[0])) {
        /* PES stream */
        if (mpeg_sys_is_valid_pes (tf, data - 3, end - data + 3, &pack_size)) {
          pes_headers++;
          if (mpegversion == 0)
            mpegversion = 2;
        }
      } else if (IS_MPEG_SYS_CODE (data[0])) {
        if (mpeg_sys_is_valid_sys (tf, data - 3, end - data + 3, &pack_size)) {
          pack_headers++;
        }
      }

      /* If we found a packet with a known size, skip the bytes in it and loop
       * around to check the next packet. */
      if (pack_size != 0) {
        data += pack_size - 3;
        sync_word = 0xffffffff;
        since_last_sync = 0;
        continue;
      }
    }

    sync_word |= data[0];
    since_last_sync++;
    data++;

    /* If we have found MAX headers, and *some* were pes headers (pack headers
     * are optional in an mpeg system stream) then return our high-probability
     * result */
    if (pes_headers > 0 && (pack_headers + pes_headers) > MPEG2_MAX_SYS_HEADERS)
      goto suggest;
  }

  /* If we at least saw MIN headers, and *some* were pes headers (pack headers
   * are optional in an mpeg system stream) then return a lower-probability 
   * result */
  if (pes_headers > 0 && (pack_headers + pes_headers) > MPEG2_MIN_SYS_HEADERS)
    goto suggest;

  return;
suggest:
  {
    guint prob;

    prob = GST_TYPE_FIND_POSSIBLE + (10 * (pack_headers + pes_headers));
    prob = MIN (prob, GST_TYPE_FIND_MAXIMUM);

    /* With the above test, we get into problems when we try to typefind
       a MPEG stream from a small amount of data, which can happen when
       we get data pushed from a HTTP source. We thus make a second test
       to give higher probability if all the potential headers were either
       pack or pes headers (ie, no potential header was unrecognized). */
    if (potential_headers == pack_headers + pes_headers) {
      GST_LOG ("Only %u headers, but all were recognized", potential_headers);
      prob += 10;
      prob = MIN (prob, GST_TYPE_FIND_MAXIMUM);
    }

    /* lower probability if the first packet wasn't right at the start */
    if (data0 != first_sync && prob >= 10)
      prob -= 10;

    GST_LOG ("Suggesting MPEG %d system stream, %d packs, %d pes, prob %u%%\n",
        mpegversion, pack_headers, pes_headers, prob);

    gst_type_find_suggest_simple (tf, prob, "video/mpeg",
        "systemstream", G_TYPE_BOOLEAN, TRUE,
        "mpegversion", G_TYPE_INT, mpegversion, NULL);
  }
};