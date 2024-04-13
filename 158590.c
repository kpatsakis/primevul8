log_write_failed(uschar *name, int length, int rc)
{
int save_errno = errno;

if (!panic_save_buffer)
  if ((panic_save_buffer = US malloc(LOG_BUFFER_SIZE)))
    memcpy(panic_save_buffer, log_buffer, LOG_BUFFER_SIZE);

log_write(0, LOG_PANIC_DIE, "failed to write to %s: length=%d result=%d "
  "errno=%d (%s)", name, length, rc, save_errno,
  (save_errno == 0)? "write incomplete" : strerror(save_errno));
/* Never returns */
}