bgp_show_neighbor (struct vty *vty, struct bgp *bgp,
		   enum show_type type, union sockunion *su)
{
  struct listnode *node, *nnode;
  struct peer *peer;
  int find = 0;

  for (ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
    {
      switch (type)
	{
	case show_all:
	  bgp_show_peer (vty, peer);
	  break;
	case show_peer:
	  if (sockunion_same (&peer->su, su))
	    {
	      find = 1;
	      bgp_show_peer (vty, peer);
	    }
	  break;
	}
    }

  if (type == show_peer && ! find)
    vty_out (vty, "%% No such neighbor%s", VTY_NEWLINE);
  
  return CMD_SUCCESS;
}