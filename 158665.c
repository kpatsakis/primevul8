smtp_write_command(void * sx, int mode, const char *format, ...)
{
smtp_outblock * outblock = &((smtp_context *)sx)->outblock;
int rc = 0;

if (format)
  {
  gstring gs = { .size = big_buffer_size, .ptr = 0, .s = big_buffer };
  va_list ap;

  va_start(ap, format);
  if (!string_vformat(&gs, FALSE, CS format, ap))
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "overlong write_command in outgoing "
      "SMTP");
  va_end(ap);
  string_from_gstring(&gs);

  if (gs.ptr > outblock->buffersize)
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "overlong write_command in outgoing "
      "SMTP");

  if (gs.ptr > outblock->buffersize - (outblock->ptr - outblock->buffer))
    {
    rc = outblock->cmd_count;                 /* flush resets */
    if (!flush_buffer(outblock, SCMD_FLUSH)) return -1;
    }

  Ustrncpy(CS outblock->ptr, gs.s, gs.ptr);
  outblock->ptr += gs.ptr;
  outblock->cmd_count++;
  gs.ptr -= 2; string_from_gstring(&gs); /* remove \r\n for error message */

  /* We want to hide the actual data sent in AUTH transactions from reflections
  and logs. While authenticating, a flag is set in the outblock to enable this.
  The AUTH command itself gets any data flattened. Other lines are flattened
  completely. */

  if (outblock->authenticating)
    {
    uschar *p = big_buffer;
    if (Ustrncmp(big_buffer, "AUTH ", 5) == 0)
      {
      p += 5;
      while (isspace(*p)) p++;
      while (!isspace(*p)) p++;
      while (isspace(*p)) p++;
      }
    while (*p != 0) *p++ = '*';
    }

  HDEBUG(D_transport|D_acl|D_v) debug_printf_indent("  SMTP>> %s\n", big_buffer);
  }

if (mode != SCMD_BUFFER)
  {
  rc += outblock->cmd_count;                /* flush resets */
  if (!flush_buffer(outblock, mode)) return -1;
  }

return rc;
}