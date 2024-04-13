write_chunk(transport_ctx * tctx, uschar *chunk, int len)
{
uschar *start = chunk;
uschar *end = chunk + len;
uschar *ptr;
int mlen = DELIVER_OUT_BUFFER_SIZE - nl_escape_length - 2;

/* The assumption is made that the check string will never stretch over move
than one chunk since the only time there are partial matches is when copying
the body in large buffers. There is always enough room in the buffer for an
escape string, since the loop below ensures this for each character it
processes, and it won't have stuck in the escape string if it left a partial
match. */

if (nl_partial_match >= 0)
  {
  if (nl_check_length > 0 && len >= nl_check_length &&
      Ustrncmp(start, nl_check + nl_partial_match,
        nl_check_length - nl_partial_match) == 0)
    {
    Ustrncpy(chunk_ptr, nl_escape, nl_escape_length);
    chunk_ptr += nl_escape_length;
    start += nl_check_length - nl_partial_match;
    }

  /* The partial match was a false one. Insert the characters carried over
  from the previous chunk. */

  else if (nl_partial_match > 0)
    {
    Ustrncpy(chunk_ptr, nl_check, nl_partial_match);
    chunk_ptr += nl_partial_match;
    }

  nl_partial_match = -1;
  }

/* Now process the characters in the chunk. Whenever we hit a newline we check
for possible escaping. The code for the non-NL route should be as fast as
possible. */

for (ptr = start; ptr < end; ptr++)
  {
  int ch, len;

  /* Flush the buffer if it has reached the threshold - we want to leave enough
  room for the next uschar, plus a possible extra CR for an LF, plus the escape
  string. */

  if ((len = chunk_ptr - deliver_out_buffer) > mlen)
    {
    DEBUG(D_transport) debug_printf("flushing headers buffer\n");

    /* If CHUNKING, prefix with BDAT (size) NON-LAST.  Also, reap responses
    from previous SMTP commands. */

    if (tctx->options & topt_use_bdat  &&  tctx->chunk_cb)
      {
      if (  tctx->chunk_cb(tctx, (unsigned)len, 0) != OK
	 || !transport_write_block(tctx, deliver_out_buffer, len, FALSE)
	 || tctx->chunk_cb(tctx, 0, tc_reap_prev) != OK
	 )
	return FALSE;
      }
    else
      if (!transport_write_block(tctx, deliver_out_buffer, len, FALSE))
	return FALSE;
    chunk_ptr = deliver_out_buffer;
    }

  /* Remove CR before NL if required */

  if (  *ptr == '\r' && ptr[1] == '\n'
     && !(tctx->options & topt_use_crlf)
     && f.spool_file_wireformat
     )
    ptr++;

  if ((ch = *ptr) == '\n')
    {
    int left = end - ptr - 1;  /* count of chars left after NL */

    /* Insert CR before NL if required */

    if (tctx->options & topt_use_crlf && !f.spool_file_wireformat)
      *chunk_ptr++ = '\r';
    *chunk_ptr++ = '\n';
    transport_newlines++;

    /* The check_string test (formerly "from hack") replaces the specific
    string at the start of a line with an escape string (e.g. "From " becomes
    ">From " or "." becomes "..". It is a case-sensitive test. The length
    check above ensures there is always enough room to insert this string. */

    if (nl_check_length > 0)
      {
      if (left >= nl_check_length &&
          Ustrncmp(ptr+1, nl_check, nl_check_length) == 0)
        {
        Ustrncpy(chunk_ptr, nl_escape, nl_escape_length);
        chunk_ptr += nl_escape_length;
        ptr += nl_check_length;
        }

      /* Handle the case when there isn't enough left to match the whole
      check string, but there may be a partial match. We remember how many
      characters matched, and finish processing this chunk. */

      else if (left <= 0) nl_partial_match = 0;

      else if (Ustrncmp(ptr+1, nl_check, left) == 0)
        {
        nl_partial_match = left;
        ptr = end;
        }
      }
    }

  /* Not a NL character */

  else *chunk_ptr++ = ch;
  }

return TRUE;
}