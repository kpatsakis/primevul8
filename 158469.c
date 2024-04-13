host_and_ident(BOOL useflag)
{
if (sender_fullhost == NULL)
  {
  (void)string_format(big_buffer, big_buffer_size, "%s%s", useflag? "U=" : "",
     (sender_ident == NULL)? US"unknown" : sender_ident);
  }
else
  {
  uschar *flag = useflag? US"H=" : US"";
  uschar *iface = US"";
  if (LOGGING(incoming_interface) && interface_address != NULL)
    iface = string_sprintf(" I=[%s]:%d", interface_address, interface_port);
  if (sender_ident == NULL)
    (void)string_format(big_buffer, big_buffer_size, "%s%s%s",
      flag, sender_fullhost, iface);
  else
    (void)string_format(big_buffer, big_buffer_size, "%s%s%s U=%s",
      flag, sender_fullhost, iface, sender_ident);
  }
return big_buffer;
}