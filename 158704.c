lmtp_write_command(int fd, const char *format, ...)
{
gstring gs = { .size = big_buffer_size, .ptr = 0, .s = big_buffer };
int rc;
va_list ap;

va_start(ap, format);
if (!string_vformat(&gs, FALSE, CS format, ap))
  {
  va_end(ap);
  errno = ERRNO_SMTPFORMAT;
  return FALSE;
  }
va_end(ap);
DEBUG(D_transport|D_v) debug_printf("  LMTP>> %s", string_from_gstring(&gs));
rc = write(fd, gs.s, gs.ptr);
gs.ptr -= 2; string_from_gstring(&gs); /* remove \r\n for debug and error message */
if (rc > 0) return TRUE;
DEBUG(D_transport) debug_printf("write failed: %s\n", strerror(errno));
return FALSE;
}