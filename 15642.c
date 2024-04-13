bgp_clear_vty (struct vty *vty, const char *name, afi_t afi, safi_t safi,
               enum clear_sort sort, enum bgp_clear_type stype, 
               const char *arg)
{
  int ret;
  struct bgp *bgp;

  /* BGP structure lookup. */
  if (name)
    {
      bgp = bgp_lookup_by_name (name);
      if (bgp == NULL)
        {
          vty_out (vty, "Can't find BGP view %s%s", name, VTY_NEWLINE);
          return CMD_WARNING;
        }
    }
  else
    {
      bgp = bgp_get_default ();
      if (bgp == NULL)
        {
          vty_out (vty, "No BGP process is configured%s", VTY_NEWLINE);
          return CMD_WARNING;
        }
    }

  ret =  bgp_clear (vty, bgp, afi, safi, sort, stype, arg);
  if (ret < 0)
    return CMD_WARNING;

  return CMD_SUCCESS;
}