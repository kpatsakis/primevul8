DEFUN_DEPRECATED (neighbor_version,
		  neighbor_version_cmd,
		  NEIGHBOR_CMD "version (4|4-)",
		  NEIGHBOR_STR
		  NEIGHBOR_ADDR_STR
		  "Set the BGP version to match a neighbor\n"
		  "Neighbor's BGP version\n")
{
  return CMD_SUCCESS;
}