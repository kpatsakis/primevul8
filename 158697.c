check_sync(void)
{
if (!smtp_enforce_sync || !sender_host_address || f.sender_host_notsocket)
  return TRUE;

return wouldblock_reading();
}