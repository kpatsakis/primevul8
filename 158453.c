check_proxy_protocol_host()
{
int rc;

if (  sender_host_address
   && (rc = verify_check_this_host(CUSS &hosts_proxy, NULL, NULL,
                           sender_host_address, NULL)) == OK)
  {
  DEBUG(D_receive)
    debug_printf("Detected proxy protocol configured host\n");
  proxy_session = TRUE;
  }
return proxy_session;
}