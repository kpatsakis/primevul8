community_list_vty (void)
{
  install_node (&community_list_node, community_list_config_write);

  /* Community-list.  */
  install_element (CONFIG_NODE, &ip_community_list_standard_cmd);
  install_element (CONFIG_NODE, &ip_community_list_standard2_cmd);
  install_element (CONFIG_NODE, &ip_community_list_expanded_cmd);
  install_element (CONFIG_NODE, &ip_community_list_name_standard_cmd);
  install_element (CONFIG_NODE, &ip_community_list_name_standard2_cmd);
  install_element (CONFIG_NODE, &ip_community_list_name_expanded_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_standard_all_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_expanded_all_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_name_standard_all_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_name_expanded_all_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_standard_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_expanded_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_name_standard_cmd);
  install_element (CONFIG_NODE, &no_ip_community_list_name_expanded_cmd);
  install_element (VIEW_NODE, &show_ip_community_list_cmd);
  install_element (VIEW_NODE, &show_ip_community_list_arg_cmd);
  install_element (ENABLE_NODE, &show_ip_community_list_cmd);
  install_element (ENABLE_NODE, &show_ip_community_list_arg_cmd);

  /* Extcommunity-list.  */
  install_element (CONFIG_NODE, &ip_extcommunity_list_standard_cmd);
  install_element (CONFIG_NODE, &ip_extcommunity_list_standard2_cmd);
  install_element (CONFIG_NODE, &ip_extcommunity_list_expanded_cmd);
  install_element (CONFIG_NODE, &ip_extcommunity_list_name_standard_cmd);
  install_element (CONFIG_NODE, &ip_extcommunity_list_name_standard2_cmd);
  install_element (CONFIG_NODE, &ip_extcommunity_list_name_expanded_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_standard_all_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_expanded_all_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_name_standard_all_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_name_expanded_all_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_standard_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_expanded_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_name_standard_cmd);
  install_element (CONFIG_NODE, &no_ip_extcommunity_list_name_expanded_cmd);
  install_element (VIEW_NODE, &show_ip_extcommunity_list_cmd);
  install_element (VIEW_NODE, &show_ip_extcommunity_list_arg_cmd);
  install_element (ENABLE_NODE, &show_ip_extcommunity_list_cmd);
  install_element (ENABLE_NODE, &show_ip_extcommunity_list_arg_cmd);
}