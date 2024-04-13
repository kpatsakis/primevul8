static void kvp_get_ipconfig_info(char *if_name,
				 struct hv_kvp_ipaddr_value *buffer)
{
	char cmd[512];
	char dhcp_info[128];
	char *p;
	FILE *file;

	/*
	 * Get the address of default gateway (ipv4).
	 */
	sprintf(cmd, "%s %s", "ip route show dev", if_name);
	strcat(cmd, " | awk '/default/ {print $3 }'");

	/*
	 * Execute the command to gather gateway info.
	 */
	kvp_process_ipconfig_file(cmd, (char *)buffer->gate_way,
				(MAX_GATEWAY_SIZE * 2), INET_ADDRSTRLEN, 0);

	/*
	 * Get the address of default gateway (ipv6).
	 */
	sprintf(cmd, "%s %s", "ip -f inet6  route show dev", if_name);
	strcat(cmd, " | awk '/default/ {print $3 }'");

	/*
	 * Execute the command to gather gateway info (ipv6).
	 */
	kvp_process_ipconfig_file(cmd, (char *)buffer->gate_way,
				(MAX_GATEWAY_SIZE * 2), INET6_ADDRSTRLEN, 1);


	/*
	 * Gather the DNS  state.
	 * Since there is no standard way to get this information
	 * across various distributions of interest; we just invoke
	 * an external script that needs to be ported across distros
	 * of interest.
	 *
	 * Following is the expected format of the information from the script:
	 *
	 * ipaddr1 (nameserver1)
	 * ipaddr2 (nameserver2)
	 * .
	 * .
	 */

	sprintf(cmd, "%s",  "hv_get_dns_info");

	/*
	 * Execute the command to gather DNS info.
	 */
	kvp_process_ipconfig_file(cmd, (char *)buffer->dns_addr,
				(MAX_IP_ADDR_SIZE * 2), INET_ADDRSTRLEN, 0);

	/*
	 * Gather the DHCP state.
	 * We will gather this state by invoking an external script.
	 * The parameter to the script is the interface name.
	 * Here is the expected output:
	 *
	 * Enabled: DHCP enabled.
	 */

	sprintf(cmd, "%s %s", "hv_get_dhcp_info", if_name);

	file = popen(cmd, "r");
	if (file == NULL)
		return;

	p = fgets(dhcp_info, sizeof(dhcp_info), file);
	if (p == NULL) {
		pclose(file);
		return;
	}

	if (!strncmp(p, "Enabled", 7))
		buffer->dhcp_enabled = 1;
	else
		buffer->dhcp_enabled = 0;

	pclose(file);
}