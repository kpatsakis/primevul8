DEFUN (neighbor_remote_as,
       neighbor_remote_as_cmd,
       NEIGHBOR_CMD2 "remote-as <1-65535>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Specify a BGP neighbor\n"
       AS_STR)
{
  return peer_remote_as_vty (vty, argv[0], argv[1], AFI_IP, SAFI_UNICAST);
}