bgp_show_neighbor_vty (struct vty *vty, const char *name, 
                       enum show_type type, const char *ip_str)
{
  int ret;
  struct bgp *bgp;
  union sockunion su;

  if (ip_str)
    {
      ret = str2sockunion (ip_str, &su);
      if (ret < 0)
        {
          vty_out (vty, "%% Malformed address: %s%s", ip_str, VTY_NEWLINE);
          return CMD_WARNING;
        }
    }

  if (name)
    {
      bgp = bgp_lookup_by_name (name);
      
      if (! bgp)
        {
          vty_out (vty, "%% No such BGP instance exist%s", VTY_NEWLINE); 
          return CMD_WARNING;
        }

      bgp_show_neighbor (vty, bgp, type, &su);

      return CMD_SUCCESS;
    }

  bgp = bgp_get_default ();

  if (bgp)
    bgp_show_neighbor (vty, bgp, type, &su);

  return CMD_SUCCESS;
}