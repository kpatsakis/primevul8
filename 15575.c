DEFUN (neighbor_advertise_interval,
       neighbor_advertise_interval_cmd,
       NEIGHBOR_CMD "advertisement-interval <0-600>",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Minimum interval between sending BGP routing updates\n"
       "time in seconds\n")
{
  return peer_advertise_interval_vty (vty, argv[0], argv[1], 1);
}