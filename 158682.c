smtp_closedown(uschar *message)
{
if (!smtp_in || smtp_batched_input) return;
receive_swallow_smtp();
smtp_printf("421 %s\r\n", FALSE, message);

for (;;) switch(smtp_read_command(FALSE, GETC_BUFFER_UNLIMITED))
  {
  case EOF_CMD:
    return;

  case QUIT_CMD:
    smtp_printf("221 %s closing connection\r\n", FALSE, smtp_active_hostname);
    mac_smtp_fflush();
    return;

  case RSET_CMD:
    smtp_printf("250 Reset OK\r\n", FALSE);
    break;

  default:
    smtp_printf("421 %s\r\n", FALSE, message);
    break;
  }
}