DEFUN_DEPRECATED (no_neighbor_capability_route_refresh,
		  no_neighbor_capability_route_refresh_cmd,
		  NO_NEIGHBOR_CMD2 "capability route-refresh",
		  NO_STR
		  NEIGHBOR_STR
		  NEIGHBOR_ADDR_STR2
		  "Advertise capability to the peer\n"
		  "Advertise route-refresh capability to this neighbor\n")
{
  return CMD_SUCCESS;
}