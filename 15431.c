DEFUN (neighbor_capability_orf_prefix,
       neighbor_capability_orf_prefix_cmd,
       NEIGHBOR_CMD2 "capability orf prefix-list (both|send|receive)",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR2
       "Advertise capability to the peer\n"
       "Advertise ORF capability to the peer\n"
       "Advertise prefixlist ORF capability to this neighbor\n"
       "Capability to SEND and RECEIVE the ORF to/from this neighbor\n"
       "Capability to RECEIVE the ORF from this neighbor\n"
       "Capability to SEND the ORF to this neighbor\n")
{
  u_int16_t flag = 0;

  if (strncmp (argv[1], "s", 1) == 0)
    flag = PEER_FLAG_ORF_PREFIX_SM;
  else if (strncmp (argv[1], "r", 1) == 0)
    flag = PEER_FLAG_ORF_PREFIX_RM;
  else if (strncmp (argv[1], "b", 1) == 0)
    flag = PEER_FLAG_ORF_PREFIX_SM|PEER_FLAG_ORF_PREFIX_RM;
  else
    return CMD_WARNING;

  return peer_af_flag_set_vty (vty, argv[0], bgp_node_afi (vty),
			       bgp_node_safi (vty), flag);
}