check_port(uschar *address)
{
int port = host_address_extract_port(address);
if (string_is_ip_address(address, NULL) == 0)
  exim_fail("exim abandoned: \"%s\" is not an IP address\n", address);
return port;
}