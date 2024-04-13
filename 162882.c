read_line (GstRTSPConnection * conn, guint8 * buffer, guint * idx, guint size,
    gboolean block)
{
  GstRTSPResult res;

  while (TRUE) {
    guint8 c;
    guint i;

    if (conn->read_ahead == READ_AHEAD_EOH) {
      /* the last call to read_line() already determined that we have reached
       * the end of the headers, so convey that information now */
      conn->read_ahead = 0;
      break;
    } else if (conn->read_ahead == READ_AHEAD_CRLF) {
      /* the last call to read_line() left off after having read \r\n */
      c = '\n';
    } else if (conn->read_ahead == READ_AHEAD_CRLFCR) {
      /* the last call to read_line() left off after having read \r\n\r */
      c = '\r';
    } else if (conn->read_ahead != 0) {
      /* the last call to read_line() left us with a character to start with */
      c = (guint8) conn->read_ahead;
      conn->read_ahead = 0;
    } else {
      /* read the next character */
      i = 0;
      res = read_bytes (conn, &c, &i, 1, block);
      if (G_UNLIKELY (res != GST_RTSP_OK))
        return res;
    }

    /* special treatment of line endings */
    if (c == '\r' || c == '\n') {
      guint8 read_ahead;

    retry:
      /* need to read ahead one more character to know what to do... */
      i = 0;
      res = read_bytes (conn, &read_ahead, &i, 1, block);
      if (G_UNLIKELY (res != GST_RTSP_OK))
        return res;

      if (read_ahead == ' ' || read_ahead == '\t') {
        if (conn->read_ahead == READ_AHEAD_CRLFCR) {
          /* got \r\n\r followed by whitespace, treat it as a normal line
           * followed by one starting with LWS */
          conn->read_ahead = read_ahead;
          break;
        } else {
          /* got LWS, change the line ending to a space and continue */
          c = ' ';
          conn->read_ahead = read_ahead;
        }
      } else if (conn->read_ahead == READ_AHEAD_CRLFCR) {
        if (read_ahead == '\r' || read_ahead == '\n') {
          /* got \r\n\r\r or \r\n\r\n, treat it as the end of the headers */
          conn->read_ahead = READ_AHEAD_EOH;
          break;
        } else {
          /* got \r\n\r followed by something else, this is not really
           * supported since we have probably just eaten the first character
           * of the body or the next message, so just ignore the second \r
           * and live with it... */
          conn->read_ahead = read_ahead;
          break;
        }
      } else if (conn->read_ahead == READ_AHEAD_CRLF) {
        if (read_ahead == '\r') {
          /* got \r\n\r so far, need one more character... */
          conn->read_ahead = READ_AHEAD_CRLFCR;
          goto retry;
        } else if (read_ahead == '\n') {
          /* got \r\n\n, treat it as the end of the headers */
          conn->read_ahead = READ_AHEAD_EOH;
          break;
        } else {
          /* found the end of a line, keep read_ahead for the next line */
          conn->read_ahead = read_ahead;
          break;
        }
      } else if (c == read_ahead) {
        /* got double \r or \n, treat it as the end of the headers */
        conn->read_ahead = READ_AHEAD_EOH;
        break;
      } else if (c == '\r' && read_ahead == '\n') {
        /* got \r\n so far, still need more to know what to do... */
        conn->read_ahead = READ_AHEAD_CRLF;
        goto retry;
      } else {
        /* found the end of a line, keep read_ahead for the next line */
        conn->read_ahead = read_ahead;
        break;
      }
    }

    if (G_LIKELY (*idx < size - 1))
      buffer[(*idx)++] = c;
  }
  buffer[*idx] = '\0';

  return GST_RTSP_OK;
}