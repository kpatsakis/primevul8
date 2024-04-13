DEFUN_DEPRECATED (neighbor_capability_route_refresh,
		  neighbor_capability_route_refresh_cmd,
		  NEIGHBOR_CMD2 "capability route-refresh",
		  NEIGHBOR_STR
		  NEIGHBOR_ADDR_STR2
		  "Advertise capability to the peer\n"
		  "Advertise route-refresh capability to this neighbor\n")
{
  return CMD_SUCCESS;
}