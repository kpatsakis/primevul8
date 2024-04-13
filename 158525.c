smtp_get_connection_info(void)
{
const uschar * hostname = sender_fullhost
  ? sender_fullhost : sender_host_address;

if (host_checking)
  return string_sprintf("SMTP connection from %s", hostname);

if (f.sender_host_unknown || f.sender_host_notsocket)
  return string_sprintf("SMTP connection from %s", sender_ident);

if (f.is_inetd)
  return string_sprintf("SMTP connection from %s (via inetd)", hostname);

if (LOGGING(incoming_interface) && interface_address != NULL)
  return string_sprintf("SMTP connection from %s I=[%s]:%d", hostname,
    interface_address, interface_port);

return string_sprintf("SMTP connection from %s", hostname);
}