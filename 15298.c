bgp_show_rsclient_summary (struct vty *vty, struct bgp *bgp, 
                           afi_t afi, safi_t safi)
{
  struct peer *peer;
  struct listnode *node, *nnode;
  int count = 0;

  /* Header string for each address family. */
  static char header[] = "Neighbor        V    AS  Export-Policy  Import-Policy  Up/Down  State";

  for (ALL_LIST_ELEMENTS (bgp->rsclient, node, nnode, peer))
    {
      if (peer->afc[afi][safi] &&
         CHECK_FLAG (peer->af_flags[afi][safi], PEER_FLAG_RSERVER_CLIENT))
       {
         if (! count)
           {
             vty_out (vty,
                      "Route Server's BGP router identifier %s%s",
                      inet_ntoa (bgp->router_id), VTY_NEWLINE);
             vty_out (vty,
              "Route Server's local AS number %d%s", bgp->as,
                       VTY_NEWLINE);

             vty_out (vty, "%s", VTY_NEWLINE);
             vty_out (vty, "%s%s", header, VTY_NEWLINE);
           }

         count += bgp_write_rsclient_summary (vty, peer, afi, safi);
       }
    }

  if (count)
    vty_out (vty, "%sTotal number of Route Server Clients %d%s", VTY_NEWLINE,
            count, VTY_NEWLINE);
  else
    vty_out (vty, "No %s Route Server Client is configured%s",
            afi == AFI_IP ? "IPv4" : "IPv6", VTY_NEWLINE);

  return CMD_SUCCESS;
}