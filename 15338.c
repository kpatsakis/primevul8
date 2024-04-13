DEFUN (neighbor_set_peer_group,
       neighbor_set_peer_group_cmd,
       NEIGHBOR_CMD "peer-group WORD",
       NEIGHBOR_STR
       NEIGHBOR_ADDR_STR
       "Member of the peer-group\n"
       "peer-group name\n")
{
  int ret;
  as_t as;
  union sockunion su;
  struct bgp *bgp;
  struct peer_group *group;

  bgp = vty->index;

  ret = str2sockunion (argv[0], &su);
  if (ret < 0)
    {
      vty_out (vty, "%% Malformed address: %s%s", argv[0], VTY_NEWLINE);
      return CMD_WARNING;
    }

  group = peer_group_lookup (bgp, argv[1]);
  if (! group)
    {
      vty_out (vty, "%% Configure the peer-group first%s", VTY_NEWLINE);
      return CMD_WARNING;
    }

  if (peer_address_self_check (&su))
    {
      vty_out (vty, "%% Can not configure the local system as neighbor%s",
	       VTY_NEWLINE);
      return CMD_WARNING;
    }

  ret = peer_group_bind (bgp, &su, group, bgp_node_afi (vty), 
			 bgp_node_safi (vty), &as);

  if (ret == BGP_ERR_PEER_GROUP_PEER_TYPE_DIFFERENT)
    {
      vty_out (vty, "%% Peer with AS %d cannot be in this peer-group, members must be all internal or all external%s", as, VTY_NEWLINE);
      return CMD_WARNING;
    }

  return bgp_vty_return (vty, ret);
}