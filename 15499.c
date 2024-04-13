bgp_write_rsclient_summary (struct vty *vty, struct peer *rsclient,
        afi_t afi, safi_t safi)
{
  char timebuf[BGP_UPTIME_LEN];
  char rmbuf[14];
  const char *rmname;
  struct peer *peer;
  struct listnode *node, *nnode;
  int len;
  int count = 0;

  if (CHECK_FLAG (rsclient->sflags, PEER_STATUS_GROUP))
    {
      for (ALL_LIST_ELEMENTS (rsclient->group->peer, node, nnode, peer))
        {
          count++;
          bgp_write_rsclient_summary (vty, peer, afi, safi);
        }
      return count;
    }

  len = vty_out (vty, "%s", rsclient->host);
  len = 16 - len;

  if (len < 1)
    vty_out (vty, "%s%*s", VTY_NEWLINE, 16, " ");
  else
    vty_out (vty, "%*s", len, " ");

  vty_out (vty, "4 ");

  vty_out (vty, "%5d ", rsclient->as);

  rmname = ROUTE_MAP_EXPORT_NAME(&rsclient->filter[afi][safi]);
  if ( rmname && strlen (rmname) > 13 )
    {
      sprintf (rmbuf, "%13s", "...");
      rmname = strncpy (rmbuf, rmname, 10);
    }
  else if (! rmname)
    rmname = "<none>";
  vty_out (vty, " %13s ", rmname);

  rmname = ROUTE_MAP_IMPORT_NAME(&rsclient->filter[afi][safi]);
  if ( rmname && strlen (rmname) > 13 )
    {
      sprintf (rmbuf, "%13s", "...");
      rmname = strncpy (rmbuf, rmname, 10);
    }
  else if (! rmname)
    rmname = "<none>";
  vty_out (vty, " %13s ", rmname);

  vty_out (vty, "%8s", peer_uptime (rsclient->uptime, timebuf, BGP_UPTIME_LEN));

  if (CHECK_FLAG (rsclient->flags, PEER_FLAG_SHUTDOWN))
    vty_out (vty, " Idle (Admin)");
  else if (CHECK_FLAG (rsclient->sflags, PEER_STATUS_PREFIX_OVERFLOW))
    vty_out (vty, " Idle (PfxCt)");
  else
    vty_out (vty, " %-11s", LOOKUP(bgp_status_msg, rsclient->status));

  vty_out (vty, "%s", VTY_NEWLINE);

  return 1;
}