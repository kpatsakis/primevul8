fetch_from_buf_http(buf_t *buf,
                    char **headers_out, size_t max_headerlen,
                    char **body_out, size_t *body_used, size_t max_bodylen,
                    int force_complete)
{
  char *headers, *p;
  size_t headerlen, bodylen, contentlen;
  int crlf_offset;

  check();
  if (!buf->head)
    return 0;

  crlf_offset = buf_find_string_offset(buf, "\r\n\r\n", 4);
  if (crlf_offset > (int)max_headerlen ||
      (crlf_offset < 0 && buf->datalen > max_headerlen)) {
    log_debug(LD_HTTP,"headers too long.");
    return -1;
  } else if (crlf_offset < 0) {
    log_debug(LD_HTTP,"headers not all here yet.");
    return 0;
  }
  /* Okay, we have a full header.  Make sure it all appears in the first
   * chunk. */
  if ((int)buf->head->datalen < crlf_offset + 4)
    buf_pullup(buf, crlf_offset+4, 0);
  headerlen = crlf_offset + 4;

  headers = buf->head->data;
  bodylen = buf->datalen - headerlen;
  log_debug(LD_HTTP,"headerlen %d, bodylen %d.", (int)headerlen, (int)bodylen);

  if (max_headerlen <= headerlen) {
    log_warn(LD_HTTP,"headerlen %d larger than %d. Failing.",
             (int)headerlen, (int)max_headerlen-1);
    return -1;
  }
  if (max_bodylen <= bodylen) {
    log_warn(LD_HTTP,"bodylen %d larger than %d. Failing.",
             (int)bodylen, (int)max_bodylen-1);
    return -1;
  }

#define CONTENT_LENGTH "\r\nContent-Length: "
  p = (char*) tor_memstr(headers, headerlen, CONTENT_LENGTH);
  if (p) {
    int i;
    i = atoi(p+strlen(CONTENT_LENGTH));
    if (i < 0) {
      log_warn(LD_PROTOCOL, "Content-Length is less than zero; it looks like "
               "someone is trying to crash us.");
      return -1;
    }
    contentlen = i;
    /* if content-length is malformed, then our body length is 0. fine. */
    log_debug(LD_HTTP,"Got a contentlen of %d.",(int)contentlen);
    if (bodylen < contentlen) {
      if (!force_complete) {
        log_debug(LD_HTTP,"body not all here yet.");
        return 0; /* not all there yet */
      }
    }
    if (bodylen > contentlen) {
      bodylen = contentlen;
      log_debug(LD_HTTP,"bodylen reduced to %d.",(int)bodylen);
    }
  }
  /* all happy. copy into the appropriate places, and return 1 */
  if (headers_out) {
    *headers_out = tor_malloc(headerlen+1);
    fetch_from_buf(*headers_out, headerlen, buf);
    (*headers_out)[headerlen] = 0; /* NUL terminate it */
  }
  if (body_out) {
    tor_assert(body_used);
    *body_used = bodylen;
    *body_out = tor_malloc(bodylen+1);
    fetch_from_buf(*body_out, bodylen, buf);
    (*body_out)[bodylen] = 0; /* NUL terminate it */
  }
  check();
  return 1;
}