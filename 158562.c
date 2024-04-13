pipeline_connect_sends(void)
{
if (!sender_host_address || f.sender_host_notsocket || !fl.pipe_connect_acceptable)
  return FALSE;

if (wouldblock_reading()) return FALSE;
f.smtp_in_early_pipe_used = TRUE;
return TRUE;
}