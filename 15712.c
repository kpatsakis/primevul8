bgp_show_summary_vty (struct vty *vty, const char *name, 
                      afi_t afi, safi_t safi)
{
  struct bgp *bgp;

  if (name)
    {
      bgp = bgp_lookup_by_name (name);
      
      if (! bgp)
	{
	  vty_out (vty, "%% No such BGP instance exist%s", VTY_NEWLINE); 
	  return CMD_WARNING;
	}

      bgp_show_summary (vty, bgp, afi, safi);
      return CMD_SUCCESS;
    }
  
  bgp = bgp_get_default ();

  if (bgp)
    bgp_show_summary (vty, bgp, afi, safi);    
 
  return CMD_SUCCESS;
}