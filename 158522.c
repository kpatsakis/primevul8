pipeline_response(void)
{
if (  !smtp_enforce_sync || !sender_host_address
   || f.sender_host_notsocket || !f.smtp_in_pipelining_advertised)
  return FALSE;

if (wouldblock_reading()) return FALSE;
f.smtp_in_pipelining_used = TRUE;
return TRUE;
}