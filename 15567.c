peer_remote_as_vty (struct vty *vty, const char *peer_str, 
                    const char *as_str, afi_t afi, safi_t safi)
{
  int ret;
  struct bgp *bgp;
  as_t as;
  union sockunion su;

  bgp = vty->index;

  /* Get AS number.  */
  VTY_GET_INTEGER_RANGE ("AS", as, as_str, 1, 65535);

  /* If peer is peer group, call proper function.  */
  ret = str2sockunion (peer_str, &su);
  if (ret < 0)
    {
      ret = peer_group_remote_as (bgp, peer_str, &as);
      if (ret < 0)
	{
	  vty_out (vty, "%% Create the peer-group first%s", VTY_NEWLINE);
	  return CMD_WARNING;
	}
      return CMD_SUCCESS;
    }

  if (peer_address_self_check (&su))
    {
      vty_out (vty, "%% Can not configure the local system as neighbor%s",
	       VTY_NEWLINE);
      return CMD_WARNING;
    }

  ret = peer_remote_as (bgp, &su, &as, afi, safi);

  /* This peer belongs to peer group.  */
  switch (ret)
    {
    case BGP_ERR_PEER_GROUP_MEMBER:
      vty_out (vty, "%% Peer-group AS %d. Cannot configure remote-as for member%s", as, VTY_NEWLINE);
      return CMD_WARNING;
    case BGP_ERR_PEER_GROUP_PEER_TYPE_DIFFERENT:
      vty_out (vty, "%% The AS# can not be changed from %d to %s, peer-group members must be all internal or all external%s", as, as_str, VTY_NEWLINE);
      return CMD_WARNING;
    }
  return bgp_vty_return (vty, ret);
}