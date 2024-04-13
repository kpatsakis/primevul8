DEFUN (address_family_ipv6_safi,
       address_family_ipv6_safi_cmd,
       "address-family ipv6 (unicast|multicast)",
       "Enter Address Family command mode\n"
       "Address family\n"
       "Address Family modifier\n"
       "Address Family modifier\n")
{
  if (strncmp (argv[0], "m", 1) == 0)
    vty->node = BGP_IPV6M_NODE;
  else
    vty->node = BGP_IPV6_NODE;

  return CMD_SUCCESS;
}