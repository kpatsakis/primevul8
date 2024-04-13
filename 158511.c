transport_write_string(int fd, const char *format, ...)
{
transport_ctx tctx = {{0}};
gstring gs = { .size = big_buffer_size, .ptr = 0, .s = big_buffer };
va_list ap;

va_start(ap, format);
if (!string_vformat(&gs, FALSE, format, ap))
  log_write(0, LOG_MAIN|LOG_PANIC_DIE, "overlong formatted string in transport");
va_end(ap);
tctx.u.fd = fd;
return transport_write_block(&tctx, gs.s, gs.ptr, FALSE);
}