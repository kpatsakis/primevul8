internal_transport_write_message(transport_ctx * tctx, int size_limit)
{
int len, size = 0;

/* Initialize pointer in output buffer. */

transport_write_reset(tctx->options);

/* Set up the data for start-of-line data checking and escaping */

if (tctx->check_string && tctx->escape_string)
  {
  nl_check = tctx->check_string;
  nl_check_length = Ustrlen(nl_check);
  nl_escape = tctx->escape_string;
  nl_escape_length = Ustrlen(nl_escape);
  }

/* Whether the escaping mechanism is applied to headers or not is controlled by
an option (set for SMTP, not otherwise). Negate the length if not wanted till
after the headers. */

if (!(tctx->options & topt_escape_headers))
  nl_check_length = -nl_check_length;

/* Write the headers if required, including any that have to be added. If there
are header rewriting rules, apply them.  The datasource is not the -D spoolfile
so temporarily hide the global that adjusts for its format. */

if (!(tctx->options & topt_no_headers))
  {
  BOOL save_wireformat = f.spool_file_wireformat;
  f.spool_file_wireformat = FALSE;

  /* Add return-path: if requested. */

  if (tctx->options & topt_add_return_path)
    {
    uschar buffer[ADDRESS_MAXLENGTH + 20];
    int n = sprintf(CS buffer, "Return-path: <%.*s>\n", ADDRESS_MAXLENGTH,
      return_path);
    if (!write_chunk(tctx, buffer, n)) goto bad;
    }

  /* Add envelope-to: if requested */

  if (tctx->options & topt_add_envelope_to)
    {
    BOOL first = TRUE;
    address_item *p;
    struct aci *plist = NULL;
    struct aci *dlist = NULL;
    void *reset_point = store_get(0);

    if (!write_chunk(tctx, US"Envelope-to: ", 13)) goto bad;

    /* Pick up from all the addresses. The plist and dlist variables are
    anchors for lists of addresses already handled; they have to be defined at
    this level because write_env_to() calls itself recursively. */

    for (p = tctx->addr; p; p = p->next)
      if (!write_env_to(p, &plist, &dlist, &first, tctx)) goto bad;

    /* Add a final newline and reset the store used for tracking duplicates */

    if (!write_chunk(tctx, US"\n", 1)) goto bad;
    store_reset(reset_point);
    }

  /* Add delivery-date: if requested. */

  if (tctx->options & topt_add_delivery_date)
    {
    uschar * s = tod_stamp(tod_full);

    if (  !write_chunk(tctx, US"Delivery-date: ", 15)
       || !write_chunk(tctx, s, Ustrlen(s))
       || !write_chunk(tctx, US"\n", 1)) goto bad;
    }

  /* Then the message's headers. Don't write any that are flagged as "old";
  that means they were rewritten, or are a record of envelope rewriting, or
  were removed (e.g. Bcc). If remove_headers is not null, skip any headers that
  match any entries therein. Then check addr->prop.remove_headers too, provided that
  addr is not NULL. */

  if (!transport_headers_send(tctx, &write_chunk))
    {
bad:
    f.spool_file_wireformat = save_wireformat;
    return FALSE;
    }

  f.spool_file_wireformat = save_wireformat;
  }

/* When doing RFC3030 CHUNKING output, work out how much data would be in a
last-BDAT, consisting of the current write_chunk() output buffer fill
(optimally, all of the headers - but it does not matter if we already had to
flush that buffer with non-last BDAT prependix) plus the amount of body data
(as expanded for CRLF lines).  Then create and write BDAT(s), and ensure
that further use of write_chunk() will not prepend BDATs.
The first BDAT written will also first flush any outstanding MAIL and RCPT
commands which were buffered thans to PIPELINING.
Commands go out (using a send()) from a different buffer to data (using a
write()).  They might not end up in the same TCP segment, which is
suboptimal. */

if (tctx->options & topt_use_bdat)
  {
  off_t fsize;
  int hsize;

  if ((hsize = chunk_ptr - deliver_out_buffer) < 0)
    hsize = 0;
  if (!(tctx->options & topt_no_body))
    {
    if ((fsize = lseek(deliver_datafile, 0, SEEK_END)) < 0) return FALSE;
    fsize -= SPOOL_DATA_START_OFFSET;
    if (size_limit > 0  &&  fsize > size_limit)
      fsize = size_limit;
    size = hsize + fsize;
    if (tctx->options & topt_use_crlf  &&  !f.spool_file_wireformat)
      size += body_linecount;	/* account for CRLF-expansion */

    /* With topt_use_bdat we never do dot-stuffing; no need to
    account for any expansion due to that. */
    }

  /* If the message is large, emit first a non-LAST chunk with just the
  headers, and reap the command responses.  This lets us error out early
  on RCPT rejects rather than sending megabytes of data.  Include headers
  on the assumption they are cheap enough and some clever implementations
  might errorcheck them too, on-the-fly, and reject that chunk. */

  if (size > DELIVER_OUT_BUFFER_SIZE && hsize > 0)
    {
    DEBUG(D_transport)
      debug_printf("sending small initial BDAT; hsize=%d\n", hsize);
    if (  tctx->chunk_cb(tctx, hsize, 0) != OK
       || !transport_write_block(tctx, deliver_out_buffer, hsize, FALSE)
       || tctx->chunk_cb(tctx, 0, tc_reap_prev) != OK
       )
      return FALSE;
    chunk_ptr = deliver_out_buffer;
    size -= hsize;
    }

  /* Emit a LAST datachunk command, and unmark the context for further
  BDAT commands. */

  if (tctx->chunk_cb(tctx, size, tc_chunk_last) != OK)
    return FALSE;
  tctx->options &= ~topt_use_bdat;
  }

/* If the body is required, ensure that the data for check strings (formerly
the "from hack") is enabled by negating the length if necessary. (It will be
negative in cases where it isn't to apply to the headers). Then ensure the body
is positioned at the start of its file (following the message id), then write
it, applying the size limit if required. */

/* If we have a wireformat -D file (CRNL lines, non-dotstuffed, no ending dot)
and we want to send a body without dotstuffing or ending-dot, in-clear,
then we can just dump it using sendfile.
This should get used for CHUNKING output and also for writing the -K file for
dkim signing,  when we had CHUNKING input.  */

#ifdef OS_SENDFILE
if (  f.spool_file_wireformat
   && !(tctx->options & (topt_no_body | topt_end_dot))
   && !nl_check_length
   && tls_out.active.sock != tctx->u.fd
   )
  {
  ssize_t copied = 0;
  off_t offset = SPOOL_DATA_START_OFFSET;

  /* Write out any header data in the buffer */

  if ((len = chunk_ptr - deliver_out_buffer) > 0)
    {
    if (!transport_write_block(tctx, deliver_out_buffer, len, TRUE))
      return FALSE;
    size -= len;
    }

  DEBUG(D_transport) debug_printf("using sendfile for body\n");

  while(size > 0)
    {
    if ((copied = os_sendfile(tctx->u.fd, deliver_datafile, &offset, size)) <= 0) break;
    size -= copied;
    }
  return copied >= 0;
  }
#else
DEBUG(D_transport) debug_printf("cannot use sendfile for body: no support\n");
#endif

DEBUG(D_transport)
  if (!(tctx->options & topt_no_body))
    debug_printf("cannot use sendfile for body: %s\n",
      !f.spool_file_wireformat ? "spoolfile not wireformat"
      : tctx->options & topt_end_dot ? "terminating dot wanted"
      : nl_check_length ? "dot- or From-stuffing wanted"
      : "TLS output wanted");

if (!(tctx->options & topt_no_body))
  {
  int size = size_limit;

  nl_check_length = abs(nl_check_length);
  nl_partial_match = 0;
  if (lseek(deliver_datafile, SPOOL_DATA_START_OFFSET, SEEK_SET) < 0)
    return FALSE;
  while (  (len = MAX(DELIVER_IN_BUFFER_SIZE, size)) > 0
	&& (len = read(deliver_datafile, deliver_in_buffer, len)) > 0)
    {
    if (!write_chunk(tctx, deliver_in_buffer, len))
      return FALSE;
    size -= len;
    }

  /* A read error on the body will have left len == -1 and errno set. */

  if (len != 0) return FALSE;
  }

/* Finished with the check string, and spool-format consideration */

nl_check_length = nl_escape_length = 0;
f.spool_file_wireformat = FALSE;

/* If requested, add a terminating "." line (SMTP output). */

if (tctx->options & topt_end_dot && !write_chunk(tctx, US".\n", 2))
  return FALSE;

/* Write out any remaining data in the buffer before returning. */

return (len = chunk_ptr - deliver_out_buffer) <= 0 ||
  transport_write_block(tctx, deliver_out_buffer, len, FALSE);
}