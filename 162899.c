parse_line (guint8 * buffer, GstRTSPMessage * msg)
{
  GstRTSPHeaderField field;
  gchar *line = (gchar *) buffer;
  gchar *field_name = NULL;
  gchar *value;

  if ((value = strchr (line, ':')) == NULL || value == line)
    goto parse_error;

  /* trim space before the colon */
  if (value[-1] == ' ')
    value[-1] = '\0';

  /* replace the colon with a NUL */
  *value++ = '\0';

  /* find the header */
  field = gst_rtsp_find_header_field (line);
  /* custom header not present in the list of pre-defined headers */
  if (field == GST_RTSP_HDR_INVALID)
    field_name = line;

  /* split up the value in multiple key:value pairs if it contains comma(s) */
  while (*value != '\0') {
    gchar *next_value;
    gchar *comma = NULL;
    gboolean quoted = FALSE;
    guint comment = 0;

    /* trim leading space */
    if (*value == ' ')
      value++;

    /* for headers which may not appear multiple times, and thus may not
     * contain multiple values on the same line, we can short-circuit the loop
     * below and the entire value results in just one key:value pair*/
    if (!gst_rtsp_header_allow_multiple (field))
      next_value = value + strlen (value);
    else
      next_value = value;

    /* find the next value, taking special care of quotes and comments */
    while (*next_value != '\0') {
      if ((quoted || comment != 0) && *next_value == '\\' &&
          next_value[1] != '\0')
        next_value++;
      else if (comment == 0 && *next_value == '"')
        quoted = !quoted;
      else if (!quoted && *next_value == '(')
        comment++;
      else if (comment != 0 && *next_value == ')')
        comment--;
      else if (!quoted && comment == 0) {
        /* To quote RFC 2068: "User agents MUST take special care in parsing
         * the WWW-Authenticate field value if it contains more than one
         * challenge, or if more than one WWW-Authenticate header field is
         * provided, since the contents of a challenge may itself contain a
         * comma-separated list of authentication parameters."
         *
         * What this means is that we cannot just look for an unquoted comma
         * when looking for multiple values in Proxy-Authenticate and
         * WWW-Authenticate headers. Instead we need to look for the sequence
         * "comma [space] token space token" before we can split after the
         * comma...
         */
        if (field == GST_RTSP_HDR_PROXY_AUTHENTICATE ||
            field == GST_RTSP_HDR_WWW_AUTHENTICATE) {
          if (*next_value == ',') {
            if (next_value[1] == ' ') {
              /* skip any space following the comma so we do not mistake it for
               * separating between two tokens */
              next_value++;
            }
            comma = next_value;
          } else if (*next_value == ' ' && next_value[1] != ',' &&
              next_value[1] != '=' && comma != NULL) {
            next_value = comma;
            comma = NULL;
            break;
          }
        } else if (*next_value == ',')
          break;
      }

      next_value++;
    }

    if (msg->type == GST_RTSP_MESSAGE_REQUEST && field == GST_RTSP_HDR_SESSION) {
      /* The timeout parameter is only allowed in a session response header
       * but some clients send it as part of the session request header.
       * Ignore everything from the semicolon to the end of the line. */
      next_value = value;
      while (*next_value != '\0') {
        if (*next_value == ';') {
          break;
        }
        next_value++;
      }
    }

    /* trim space */
    if (value != next_value && next_value[-1] == ' ')
      next_value[-1] = '\0';

    if (*next_value != '\0')
      *next_value++ = '\0';

    /* add the key:value pair */
    if (*value != '\0') {
      if (field != GST_RTSP_HDR_INVALID)
        gst_rtsp_message_add_header (msg, field, value);
      else
        gst_rtsp_message_add_header_by_name (msg, field_name, value);
    }

    value = next_value;
  }

  return GST_RTSP_OK;

  /* ERRORS */
parse_error:
  {
    return GST_RTSP_EPARSE;
  }
}