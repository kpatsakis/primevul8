peer_and_group_lookup_vty (struct vty *vty, const char *peer_str)
{
  int ret;
  struct bgp *bgp;
  union sockunion su;
  struct peer *peer;
  struct peer_group *group;

  bgp = vty->index;

  ret = str2sockunion (peer_str, &su);
  if (ret == 0)
    {
      peer = peer_lookup (bgp, &su);
      if (peer)
	return peer;
    }
  else
    {
      group = peer_group_lookup (bgp, peer_str);
      if (group)
	return group->conf;
    }

  vty_out (vty, "%% Specify remote-as or peer-group commands first%s",
	   VTY_NEWLINE);

  return NULL;
}