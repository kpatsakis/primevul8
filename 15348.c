bgp_vty_init (void)
{
  /* Install bgp top node. */
  install_node (&bgp_node, bgp_config_write);
  install_node (&bgp_ipv4_unicast_node, NULL);
  install_node (&bgp_ipv4_multicast_node, NULL);
  install_node (&bgp_ipv6_unicast_node, NULL);
  install_node (&bgp_ipv6_multicast_node, NULL);
  install_node (&bgp_vpnv4_node, NULL);

  /* Install default VTY commands to new nodes.  */
  install_default (BGP_NODE);
  install_default (BGP_IPV4_NODE);
  install_default (BGP_IPV4M_NODE);
  install_default (BGP_IPV6_NODE);
  install_default (BGP_IPV6M_NODE);
  install_default (BGP_VPNV4_NODE);
  
  /* "bgp multiple-instance" commands. */
  install_element (CONFIG_NODE, &bgp_multiple_instance_cmd);
  install_element (CONFIG_NODE, &no_bgp_multiple_instance_cmd);

  /* "bgp config-type" commands. */
  install_element (CONFIG_NODE, &bgp_config_type_cmd);
  install_element (CONFIG_NODE, &no_bgp_config_type_cmd);

  /* Dummy commands (Currently not supported) */
  install_element (BGP_NODE, &no_synchronization_cmd);
  install_element (BGP_NODE, &no_auto_summary_cmd);

  /* "router bgp" commands. */
  install_element (CONFIG_NODE, &router_bgp_cmd);
  install_element (CONFIG_NODE, &router_bgp_view_cmd);

  /* "no router bgp" commands. */
  install_element (CONFIG_NODE, &no_router_bgp_cmd);
  install_element (CONFIG_NODE, &no_router_bgp_view_cmd);

  /* "bgp router-id" commands. */
  install_element (BGP_NODE, &bgp_router_id_cmd);
  install_element (BGP_NODE, &no_bgp_router_id_cmd);
  install_element (BGP_NODE, &no_bgp_router_id_val_cmd);

  /* "bgp cluster-id" commands. */
  install_element (BGP_NODE, &bgp_cluster_id_cmd);
  install_element (BGP_NODE, &bgp_cluster_id32_cmd);
  install_element (BGP_NODE, &no_bgp_cluster_id_cmd);
  install_element (BGP_NODE, &no_bgp_cluster_id_arg_cmd);

  /* "bgp confederation" commands. */
  install_element (BGP_NODE, &bgp_confederation_identifier_cmd);
  install_element (BGP_NODE, &no_bgp_confederation_identifier_cmd);
  install_element (BGP_NODE, &no_bgp_confederation_identifier_arg_cmd);

  /* "bgp confederation peers" commands. */
  install_element (BGP_NODE, &bgp_confederation_peers_cmd);
  install_element (BGP_NODE, &no_bgp_confederation_peers_cmd);

  /* "timers bgp" commands. */
  install_element (BGP_NODE, &bgp_timers_cmd);
  install_element (BGP_NODE, &no_bgp_timers_cmd);
  install_element (BGP_NODE, &no_bgp_timers_arg_cmd);

  /* "bgp client-to-client reflection" commands */
  install_element (BGP_NODE, &no_bgp_client_to_client_reflection_cmd);
  install_element (BGP_NODE, &bgp_client_to_client_reflection_cmd);

  /* "bgp always-compare-med" commands */
  install_element (BGP_NODE, &bgp_always_compare_med_cmd);
  install_element (BGP_NODE, &no_bgp_always_compare_med_cmd);
  
  /* "bgp deterministic-med" commands */
  install_element (BGP_NODE, &bgp_deterministic_med_cmd);
  install_element (BGP_NODE, &no_bgp_deterministic_med_cmd);

  /* "bgp graceful-restart" commands */
  install_element (BGP_NODE, &bgp_graceful_restart_cmd);
  install_element (BGP_NODE, &no_bgp_graceful_restart_cmd);
  install_element (BGP_NODE, &bgp_graceful_restart_stalepath_time_cmd);
  install_element (BGP_NODE, &no_bgp_graceful_restart_stalepath_time_cmd);
  install_element (BGP_NODE, &no_bgp_graceful_restart_stalepath_time_val_cmd);
 
  /* "bgp fast-external-failover" commands */
  install_element (BGP_NODE, &bgp_fast_external_failover_cmd);
  install_element (BGP_NODE, &no_bgp_fast_external_failover_cmd);

  /* "bgp enforce-first-as" commands */
  install_element (BGP_NODE, &bgp_enforce_first_as_cmd);
  install_element (BGP_NODE, &no_bgp_enforce_first_as_cmd);

  /* "bgp bestpath compare-routerid" commands */
  install_element (BGP_NODE, &bgp_bestpath_compare_router_id_cmd);
  install_element (BGP_NODE, &no_bgp_bestpath_compare_router_id_cmd);

  /* "bgp bestpath as-path ignore" commands */
  install_element (BGP_NODE, &bgp_bestpath_aspath_ignore_cmd);
  install_element (BGP_NODE, &no_bgp_bestpath_aspath_ignore_cmd);

  /* "bgp bestpath as-path confed" commands */
  install_element (BGP_NODE, &bgp_bestpath_aspath_confed_cmd);
  install_element (BGP_NODE, &no_bgp_bestpath_aspath_confed_cmd);

  /* "bgp log-neighbor-changes" commands */
  install_element (BGP_NODE, &bgp_log_neighbor_changes_cmd);
  install_element (BGP_NODE, &no_bgp_log_neighbor_changes_cmd);

  /* "bgp bestpath med" commands */
  install_element (BGP_NODE, &bgp_bestpath_med_cmd);
  install_element (BGP_NODE, &bgp_bestpath_med2_cmd);
  install_element (BGP_NODE, &bgp_bestpath_med3_cmd);
  install_element (BGP_NODE, &no_bgp_bestpath_med_cmd);
  install_element (BGP_NODE, &no_bgp_bestpath_med2_cmd);
  install_element (BGP_NODE, &no_bgp_bestpath_med3_cmd);

  /* "no bgp default ipv4-unicast" commands. */
  install_element (BGP_NODE, &no_bgp_default_ipv4_unicast_cmd);
  install_element (BGP_NODE, &bgp_default_ipv4_unicast_cmd);
  
  /* "bgp network import-check" commands. */
  install_element (BGP_NODE, &bgp_network_import_check_cmd);
  install_element (BGP_NODE, &no_bgp_network_import_check_cmd);

  /* "bgp default local-preference" commands. */
  install_element (BGP_NODE, &bgp_default_local_preference_cmd);
  install_element (BGP_NODE, &no_bgp_default_local_preference_cmd);
  install_element (BGP_NODE, &no_bgp_default_local_preference_val_cmd);

  /* "neighbor remote-as" commands. */
  install_element (BGP_NODE, &neighbor_remote_as_cmd);
  install_element (BGP_NODE, &no_neighbor_cmd);
  install_element (BGP_NODE, &no_neighbor_remote_as_cmd);

  /* "neighbor peer-group" commands. */
  install_element (BGP_NODE, &neighbor_peer_group_cmd);
  install_element (BGP_NODE, &no_neighbor_peer_group_cmd);
  install_element (BGP_NODE, &no_neighbor_peer_group_remote_as_cmd);

  /* "neighbor local-as" commands. */
  install_element (BGP_NODE, &neighbor_local_as_cmd);
  install_element (BGP_NODE, &neighbor_local_as_no_prepend_cmd);
  install_element (BGP_NODE, &no_neighbor_local_as_cmd);
  install_element (BGP_NODE, &no_neighbor_local_as_val_cmd);
  install_element (BGP_NODE, &no_neighbor_local_as_val2_cmd);

  /* "neighbor activate" commands. */
  install_element (BGP_NODE, &neighbor_activate_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_activate_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_activate_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_activate_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_activate_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_activate_cmd);

  /* "no neighbor activate" commands. */
  install_element (BGP_NODE, &no_neighbor_activate_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_activate_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_activate_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_activate_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_activate_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_activate_cmd);

  /* "neighbor peer-group set" commands. */
  install_element (BGP_NODE, &neighbor_set_peer_group_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_set_peer_group_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_set_peer_group_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_set_peer_group_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_set_peer_group_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_set_peer_group_cmd);
  
  /* "no neighbor peer-group unset" commands. */
  install_element (BGP_NODE, &no_neighbor_set_peer_group_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_set_peer_group_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_set_peer_group_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_set_peer_group_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_set_peer_group_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_set_peer_group_cmd);
  
  /* "neighbor softreconfiguration inbound" commands.*/
  install_element (BGP_NODE, &neighbor_soft_reconfiguration_cmd);
  install_element (BGP_NODE, &no_neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_soft_reconfiguration_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_soft_reconfiguration_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_soft_reconfiguration_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_soft_reconfiguration_cmd);

  /* "neighbor attribute-unchanged" commands.  */
  install_element (BGP_NODE, &neighbor_attr_unchanged_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged1_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged2_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged3_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged4_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged5_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged6_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged7_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged8_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged9_cmd);
  install_element (BGP_NODE, &neighbor_attr_unchanged10_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged1_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged2_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged3_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged4_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged5_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged6_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged7_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged8_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged9_cmd);
  install_element (BGP_NODE, &no_neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_attr_unchanged10_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged1_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged2_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged3_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged4_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged5_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged6_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged7_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged8_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged9_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_attr_unchanged10_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged1_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged2_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged3_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged4_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged5_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged6_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged7_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged8_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged9_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_attr_unchanged10_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged1_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged2_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged3_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged4_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged5_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged6_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged7_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged8_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged9_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_attr_unchanged10_cmd);

  /* "nexthop-local unchanged" commands */
  install_element (BGP_IPV6_NODE, &neighbor_nexthop_local_unchanged_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_nexthop_local_unchanged_cmd);

  /* "transparent-as" and "transparent-nexthop" for old version
     compatibility.  */
  install_element (BGP_NODE, &neighbor_transparent_as_cmd);
  install_element (BGP_NODE, &neighbor_transparent_nexthop_cmd);

  /* "neighbor next-hop-self" commands. */
  install_element (BGP_NODE, &neighbor_nexthop_self_cmd);
  install_element (BGP_NODE, &no_neighbor_nexthop_self_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_nexthop_self_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_nexthop_self_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_nexthop_self_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_nexthop_self_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_nexthop_self_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_nexthop_self_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_nexthop_self_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_nexthop_self_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_nexthop_self_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_nexthop_self_cmd);

  /* "neighbor remove-private-AS" commands. */
  install_element (BGP_NODE, &neighbor_remove_private_as_cmd);
  install_element (BGP_NODE, &no_neighbor_remove_private_as_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_remove_private_as_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_remove_private_as_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_remove_private_as_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_remove_private_as_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_remove_private_as_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_remove_private_as_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_remove_private_as_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_remove_private_as_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_remove_private_as_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_remove_private_as_cmd);

  /* "neighbor send-community" commands.*/
  install_element (BGP_NODE, &neighbor_send_community_cmd);
  install_element (BGP_NODE, &neighbor_send_community_type_cmd);
  install_element (BGP_NODE, &no_neighbor_send_community_cmd);
  install_element (BGP_NODE, &no_neighbor_send_community_type_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_send_community_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_send_community_type_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_send_community_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_send_community_type_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_send_community_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_send_community_type_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_send_community_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_send_community_type_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_send_community_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_send_community_type_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_send_community_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_send_community_type_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_send_community_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_send_community_type_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_send_community_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_send_community_type_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_send_community_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_send_community_type_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_send_community_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_send_community_type_cmd);

  /* "neighbor route-reflector" commands.*/
  install_element (BGP_NODE, &neighbor_route_reflector_client_cmd);
  install_element (BGP_NODE, &no_neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_route_reflector_client_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_route_reflector_client_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_route_reflector_client_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_route_reflector_client_cmd);

  /* "neighbor route-server" commands.*/
  install_element (BGP_NODE, &neighbor_route_server_client_cmd);
  install_element (BGP_NODE, &no_neighbor_route_server_client_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_route_server_client_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_route_server_client_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_route_server_client_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_route_server_client_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_route_server_client_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_route_server_client_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_route_server_client_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_route_server_client_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_route_server_client_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_route_server_client_cmd);

  /* "neighbor passive" commands. */
  install_element (BGP_NODE, &neighbor_passive_cmd);
  install_element (BGP_NODE, &no_neighbor_passive_cmd);

  /* "neighbor shutdown" commands. */
  install_element (BGP_NODE, &neighbor_shutdown_cmd);
  install_element (BGP_NODE, &no_neighbor_shutdown_cmd);

  /* Deprecated "neighbor capability route-refresh" commands.*/
  install_element (BGP_NODE, &neighbor_capability_route_refresh_cmd);
  install_element (BGP_NODE, &no_neighbor_capability_route_refresh_cmd);

  /* "neighbor capability orf prefix-list" commands.*/
  install_element (BGP_NODE, &neighbor_capability_orf_prefix_cmd);
  install_element (BGP_NODE, &no_neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_capability_orf_prefix_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_capability_orf_prefix_cmd);

  /* "neighbor capability dynamic" commands.*/
  install_element (BGP_NODE, &neighbor_capability_dynamic_cmd);
  install_element (BGP_NODE, &no_neighbor_capability_dynamic_cmd);

  /* "neighbor dont-capability-negotiate" commands. */
  install_element (BGP_NODE, &neighbor_dont_capability_negotiate_cmd);
  install_element (BGP_NODE, &no_neighbor_dont_capability_negotiate_cmd);

  /* "neighbor ebgp-multihop" commands. */
  install_element (BGP_NODE, &neighbor_ebgp_multihop_cmd);
  install_element (BGP_NODE, &neighbor_ebgp_multihop_ttl_cmd);
  install_element (BGP_NODE, &no_neighbor_ebgp_multihop_cmd);
  install_element (BGP_NODE, &no_neighbor_ebgp_multihop_ttl_cmd);

  /* "neighbor disable-connected-check" commands.  */
  install_element (BGP_NODE, &neighbor_disable_connected_check_cmd);
  install_element (BGP_NODE, &no_neighbor_disable_connected_check_cmd);
  install_element (BGP_NODE, &neighbor_enforce_multihop_cmd);
  install_element (BGP_NODE, &no_neighbor_enforce_multihop_cmd);

  /* "neighbor description" commands. */
  install_element (BGP_NODE, &neighbor_description_cmd);
  install_element (BGP_NODE, &no_neighbor_description_cmd);
  install_element (BGP_NODE, &no_neighbor_description_val_cmd);

  /* "neighbor update-source" commands. "*/
  install_element (BGP_NODE, &neighbor_update_source_cmd);
  install_element (BGP_NODE, &no_neighbor_update_source_cmd);

  /* "neighbor default-originate" commands. */
  install_element (BGP_NODE, &neighbor_default_originate_cmd);
  install_element (BGP_NODE, &neighbor_default_originate_rmap_cmd);
  install_element (BGP_NODE, &no_neighbor_default_originate_cmd);
  install_element (BGP_NODE, &no_neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_default_originate_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_default_originate_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_default_originate_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_default_originate_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_default_originate_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_default_originate_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_default_originate_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_default_originate_rmap_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_default_originate_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_default_originate_rmap_cmd);

  /* "neighbor port" commands. */
  install_element (BGP_NODE, &neighbor_port_cmd);
  install_element (BGP_NODE, &no_neighbor_port_cmd);
  install_element (BGP_NODE, &no_neighbor_port_val_cmd);

  /* "neighbor weight" commands. */
  install_element (BGP_NODE, &neighbor_weight_cmd);
  install_element (BGP_NODE, &no_neighbor_weight_cmd);
  install_element (BGP_NODE, &no_neighbor_weight_val_cmd);

  /* "neighbor override-capability" commands. */
  install_element (BGP_NODE, &neighbor_override_capability_cmd);
  install_element (BGP_NODE, &no_neighbor_override_capability_cmd);

  /* "neighbor strict-capability-match" commands. */
  install_element (BGP_NODE, &neighbor_strict_capability_cmd);
  install_element (BGP_NODE, &no_neighbor_strict_capability_cmd);

  /* "neighbor timers" commands. */
  install_element (BGP_NODE, &neighbor_timers_cmd);
  install_element (BGP_NODE, &no_neighbor_timers_cmd);

  /* "neighbor timers connect" commands. */
  install_element (BGP_NODE, &neighbor_timers_connect_cmd);
  install_element (BGP_NODE, &no_neighbor_timers_connect_cmd);
  install_element (BGP_NODE, &no_neighbor_timers_connect_val_cmd);

  /* "neighbor advertisement-interval" commands. */
  install_element (BGP_NODE, &neighbor_advertise_interval_cmd);
  install_element (BGP_NODE, &no_neighbor_advertise_interval_cmd);
  install_element (BGP_NODE, &no_neighbor_advertise_interval_val_cmd);

  /* "neighbor version" commands. */
  install_element (BGP_NODE, &neighbor_version_cmd);

  /* "neighbor interface" commands. */
  install_element (BGP_NODE, &neighbor_interface_cmd);
  install_element (BGP_NODE, &no_neighbor_interface_cmd);

  /* "neighbor distribute" commands. */
  install_element (BGP_NODE, &neighbor_distribute_list_cmd);
  install_element (BGP_NODE, &no_neighbor_distribute_list_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_distribute_list_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_distribute_list_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_distribute_list_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_distribute_list_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_distribute_list_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_distribute_list_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_distribute_list_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_distribute_list_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_distribute_list_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_distribute_list_cmd);

  /* "neighbor prefix-list" commands. */
  install_element (BGP_NODE, &neighbor_prefix_list_cmd);
  install_element (BGP_NODE, &no_neighbor_prefix_list_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_prefix_list_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_prefix_list_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_prefix_list_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_prefix_list_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_prefix_list_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_prefix_list_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_prefix_list_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_prefix_list_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_prefix_list_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_prefix_list_cmd);

  /* "neighbor filter-list" commands. */
  install_element (BGP_NODE, &neighbor_filter_list_cmd);
  install_element (BGP_NODE, &no_neighbor_filter_list_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_filter_list_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_filter_list_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_filter_list_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_filter_list_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_filter_list_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_filter_list_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_filter_list_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_filter_list_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_filter_list_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_filter_list_cmd);

  /* "neighbor route-map" commands. */
  install_element (BGP_NODE, &neighbor_route_map_cmd);
  install_element (BGP_NODE, &no_neighbor_route_map_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_route_map_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_route_map_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_route_map_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_route_map_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_route_map_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_route_map_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_route_map_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_route_map_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_route_map_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_route_map_cmd);

  /* "neighbor unsuppress-map" commands. */
  install_element (BGP_NODE, &neighbor_unsuppress_map_cmd);
  install_element (BGP_NODE, &no_neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_unsuppress_map_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_unsuppress_map_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_unsuppress_map_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_unsuppress_map_cmd);  

  /* "neighbor maximum-prefix" commands. */
  install_element (BGP_NODE, &neighbor_maximum_prefix_cmd);
  install_element (BGP_NODE, &neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_NODE, &neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_NODE, &neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_NODE, &neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_NODE, &neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_val_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_NODE, &no_neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_val_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_val_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_val_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_val_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_maximum_prefix_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_maximum_prefix_threshold_restart_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_val_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_threshold_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_warning_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_threshold_warning_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_restart_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_maximum_prefix_threshold_restart_cmd);

  /* "neighbor allowas-in" */
  install_element (BGP_NODE, &neighbor_allowas_in_cmd);
  install_element (BGP_NODE, &neighbor_allowas_in_arg_cmd);
  install_element (BGP_NODE, &no_neighbor_allowas_in_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_allowas_in_cmd);
  install_element (BGP_IPV4_NODE, &neighbor_allowas_in_arg_cmd);
  install_element (BGP_IPV4_NODE, &no_neighbor_allowas_in_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_allowas_in_cmd);
  install_element (BGP_IPV4M_NODE, &neighbor_allowas_in_arg_cmd);
  install_element (BGP_IPV4M_NODE, &no_neighbor_allowas_in_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_allowas_in_cmd);
  install_element (BGP_IPV6_NODE, &neighbor_allowas_in_arg_cmd);
  install_element (BGP_IPV6_NODE, &no_neighbor_allowas_in_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_allowas_in_cmd);
  install_element (BGP_IPV6M_NODE, &neighbor_allowas_in_arg_cmd);
  install_element (BGP_IPV6M_NODE, &no_neighbor_allowas_in_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_allowas_in_cmd);
  install_element (BGP_VPNV4_NODE, &neighbor_allowas_in_arg_cmd);
  install_element (BGP_VPNV4_NODE, &no_neighbor_allowas_in_cmd);

  /* address-family commands. */
  install_element (BGP_NODE, &address_family_ipv4_cmd);
  install_element (BGP_NODE, &address_family_ipv4_safi_cmd);
#ifdef HAVE_IPV6
  install_element (BGP_NODE, &address_family_ipv6_cmd);
  install_element (BGP_NODE, &address_family_ipv6_safi_cmd);
#endif /* HAVE_IPV6 */
  install_element (BGP_NODE, &address_family_vpnv4_cmd);
  install_element (BGP_NODE, &address_family_vpnv4_unicast_cmd);

  /* "exit-address-family" command. */
  install_element (BGP_IPV4_NODE, &exit_address_family_cmd);
  install_element (BGP_IPV4M_NODE, &exit_address_family_cmd);
  install_element (BGP_IPV6_NODE, &exit_address_family_cmd);
  install_element (BGP_IPV6M_NODE, &exit_address_family_cmd);
  install_element (BGP_VPNV4_NODE, &exit_address_family_cmd);

  /* "clear ip bgp commands" */
  install_element (ENABLE_NODE, &clear_ip_bgp_all_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_cmd);
#ifdef HAVE_IPV6
  install_element (ENABLE_NODE, &clear_bgp_all_cmd);
  install_element (ENABLE_NODE, &clear_bgp_instance_all_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_cmd);
#endif /* HAVE_IPV6 */

  /* "clear ip bgp neighbor soft in" */
  install_element (ENABLE_NODE, &clear_ip_bgp_all_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_ipv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_ipv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_ipv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_ipv4_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_ipv4_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_ipv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_ipv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_ipv4_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_ipv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_ipv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_ipv4_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_ipv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_ipv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_ipv4_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_ipv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_ipv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_ipv4_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_vpnv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_vpnv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_vpnv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_vpnv4_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_vpnv4_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_vpnv4_in_cmd);
#ifdef HAVE_IPV6
  install_element (ENABLE_NODE, &clear_bgp_all_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_instance_all_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_all_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_all_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_in_prefix_filter_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_soft_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_in_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_in_prefix_filter_cmd);
#endif /* HAVE_IPV6 */

  /* "clear ip bgp neighbor soft out" */
  install_element (ENABLE_NODE, &clear_ip_bgp_all_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_ipv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_ipv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_ipv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_ipv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_ipv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_ipv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_ipv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_ipv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_ipv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_ipv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_ipv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_vpnv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_vpnv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_vpnv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_vpnv4_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_vpnv4_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_vpnv4_out_cmd);
#ifdef HAVE_IPV6
  install_element (ENABLE_NODE, &clear_bgp_all_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_instance_all_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_all_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_soft_out_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_out_cmd);
#endif /* HAVE_IPV6 */

  /* "clear ip bgp neighbor soft" */
  install_element (ENABLE_NODE, &clear_ip_bgp_all_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_ipv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_ipv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_ipv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_group_ipv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_external_ipv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_ipv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_all_vpnv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_vpnv4_soft_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_as_vpnv4_soft_cmd);
#ifdef HAVE_IPV6
  install_element (ENABLE_NODE, &clear_bgp_all_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_instance_all_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_group_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_external_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_as_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_group_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_external_soft_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_as_soft_cmd);
#endif /* HAVE_IPV6 */

  /* "clear ip bgp neighbor rsclient" */
  install_element (ENABLE_NODE, &clear_ip_bgp_all_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_all_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_peer_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_ip_bgp_instance_peer_rsclient_cmd);
#ifdef HAVE_IPV6
  install_element (ENABLE_NODE, &clear_bgp_all_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_instance_all_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_all_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_instance_all_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_peer_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_instance_peer_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_peer_rsclient_cmd);
  install_element (ENABLE_NODE, &clear_bgp_ipv6_instance_peer_rsclient_cmd);
#endif /* HAVE_IPV6 */

  /* "show ip bgp summary" commands. */
  install_element (VIEW_NODE, &show_ip_bgp_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_instance_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_ipv4_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_instance_ipv4_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_summary_cmd);
#ifdef HAVE_IPV6
  install_element (VIEW_NODE, &show_bgp_summary_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_summary_cmd);
  install_element (VIEW_NODE, &show_bgp_ipv6_summary_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_ipv6_summary_cmd);
#endif /* HAVE_IPV6 */
  install_element (ENABLE_NODE, &show_ip_bgp_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_instance_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_ipv4_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_instance_ipv4_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_summary_cmd);
#ifdef HAVE_IPV6
  install_element (ENABLE_NODE, &show_bgp_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_ipv6_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_ipv6_summary_cmd);
#endif /* HAVE_IPV6 */

  /* "show ip bgp neighbors" commands. */
  install_element (VIEW_NODE, &show_ip_bgp_neighbors_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_ipv4_neighbors_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_ipv4_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_neighbors_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_neighbors_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_all_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_vpnv4_rd_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_instance_neighbors_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_instance_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_neighbors_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_ipv4_neighbors_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_ipv4_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_neighbors_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_neighbors_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_all_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_vpnv4_rd_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_instance_neighbors_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_instance_neighbors_peer_cmd);

#ifdef HAVE_IPV6
  install_element (VIEW_NODE, &show_bgp_neighbors_cmd);
  install_element (VIEW_NODE, &show_bgp_ipv6_neighbors_cmd);
  install_element (VIEW_NODE, &show_bgp_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_bgp_ipv6_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_neighbors_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_ipv6_neighbors_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_neighbors_peer_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_ipv6_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_bgp_neighbors_cmd);
  install_element (ENABLE_NODE, &show_bgp_ipv6_neighbors_cmd);
  install_element (ENABLE_NODE, &show_bgp_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_bgp_ipv6_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_neighbors_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_ipv6_neighbors_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_neighbors_peer_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_ipv6_neighbors_peer_cmd);

  /* Old commands.  */
  install_element (VIEW_NODE, &show_ipv6_bgp_summary_cmd);
  install_element (VIEW_NODE, &show_ipv6_mbgp_summary_cmd);
  install_element (ENABLE_NODE, &show_ipv6_bgp_summary_cmd);
  install_element (ENABLE_NODE, &show_ipv6_mbgp_summary_cmd);
#endif /* HAVE_IPV6 */

  /* "show ip bgp rsclient" commands. */
  install_element (VIEW_NODE, &show_ip_bgp_rsclient_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_instance_rsclient_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_ipv4_rsclient_summary_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_instance_ipv4_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_instance_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_ipv4_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_instance_ipv4_rsclient_summary_cmd);

#ifdef HAVE_IPV6
  install_element (VIEW_NODE, &show_bgp_rsclient_summary_cmd);
  install_element (VIEW_NODE, &show_bgp_ipv6_rsclient_summary_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_rsclient_summary_cmd);
  install_element (VIEW_NODE, &show_bgp_instance_ipv6_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_ipv6_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_rsclient_summary_cmd);
  install_element (ENABLE_NODE, &show_bgp_instance_ipv6_rsclient_summary_cmd);
#endif /* HAVE_IPV6 */

  /* "show ip bgp paths" commands. */
  install_element (VIEW_NODE, &show_ip_bgp_paths_cmd);
  install_element (VIEW_NODE, &show_ip_bgp_ipv4_paths_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_paths_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_ipv4_paths_cmd);

  /* "show ip bgp community" commands. */
  install_element (VIEW_NODE, &show_ip_bgp_community_info_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_community_info_cmd);

  /* "show ip bgp attribute-info" commands. */
  install_element (VIEW_NODE, &show_ip_bgp_attr_info_cmd);
  install_element (ENABLE_NODE, &show_ip_bgp_attr_info_cmd);

  /* "redistribute" commands.  */
  install_element (BGP_NODE, &bgp_redistribute_ipv4_cmd);
  install_element (BGP_NODE, &no_bgp_redistribute_ipv4_cmd);
  install_element (BGP_NODE, &bgp_redistribute_ipv4_rmap_cmd);
  install_element (BGP_NODE, &no_bgp_redistribute_ipv4_rmap_cmd);
  install_element (BGP_NODE, &bgp_redistribute_ipv4_metric_cmd);
  install_element (BGP_NODE, &no_bgp_redistribute_ipv4_metric_cmd);
  install_element (BGP_NODE, &bgp_redistribute_ipv4_rmap_metric_cmd);
  install_element (BGP_NODE, &bgp_redistribute_ipv4_metric_rmap_cmd);
  install_element (BGP_NODE, &no_bgp_redistribute_ipv4_rmap_metric_cmd);
  install_element (BGP_NODE, &no_bgp_redistribute_ipv4_metric_rmap_cmd);
#ifdef HAVE_IPV6
  install_element (BGP_IPV6_NODE, &bgp_redistribute_ipv6_cmd);
  install_element (BGP_IPV6_NODE, &no_bgp_redistribute_ipv6_cmd);
  install_element (BGP_IPV6_NODE, &bgp_redistribute_ipv6_rmap_cmd);
  install_element (BGP_IPV6_NODE, &no_bgp_redistribute_ipv6_rmap_cmd);
  install_element (BGP_IPV6_NODE, &bgp_redistribute_ipv6_metric_cmd);
  install_element (BGP_IPV6_NODE, &no_bgp_redistribute_ipv6_metric_cmd);
  install_element (BGP_IPV6_NODE, &bgp_redistribute_ipv6_rmap_metric_cmd);
  install_element (BGP_IPV6_NODE, &bgp_redistribute_ipv6_metric_rmap_cmd);
  install_element (BGP_IPV6_NODE, &no_bgp_redistribute_ipv6_rmap_metric_cmd);
  install_element (BGP_IPV6_NODE, &no_bgp_redistribute_ipv6_metric_rmap_cmd);
#endif /* HAVE_IPV6 */

  /* "show bgp memory" commands. */
  install_element (VIEW_NODE, &show_bgp_memory_cmd);
  install_element (ENABLE_NODE, &show_bgp_memory_cmd);
  
  /* Community-list. */
  community_list_vty ();
}