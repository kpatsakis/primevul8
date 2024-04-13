DEFUN (address_family_ipv4,
       address_family_ipv4_cmd,
       "address-family ipv4",
       "Enter Address Family command mode\n"
       "Address family\n")
{
  vty->node = BGP_IPV4_NODE;
  return CMD_SUCCESS;
}