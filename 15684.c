DEFUN (address_family_ipv6,
       address_family_ipv6_cmd,
       "address-family ipv6",
       "Enter Address Family command mode\n"
       "Address family\n")
{
  vty->node = BGP_IPV6_NODE;
  return CMD_SUCCESS;
}