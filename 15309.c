DEFUN (address_family_vpnv4,
       address_family_vpnv4_cmd,
       "address-family vpnv4",
       "Enter Address Family command mode\n"
       "Address family\n")
{
  vty->node = BGP_VPNV4_NODE;
  return CMD_SUCCESS;
}