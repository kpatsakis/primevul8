static int kvp_set_ip_info(char *if_name, struct hv_kvp_ipaddr_value *new_val)
{
	int error = 0;
	char if_file[128];
	FILE *file;
	char cmd[512];
	char *mac_addr;

	/*
	 * Set the configuration for the specified interface with
	 * the information provided. Since there is no standard
	 * way to configure an interface, we will have an external
	 * script that does the job of configuring the interface and
	 * flushing the configuration.
	 *
	 * The parameters passed to this external script are:
	 * 1. A configuration file that has the specified configuration.
	 *
	 * We will embed the name of the interface in the configuration
	 * file: ifcfg-ethx (where ethx is the interface name).
	 *
	 * The information provided here may be more than what is needed
	 * in a given distro to configure the interface and so are free
	 * ignore information that may not be relevant.
	 *
	 * Here is the format of the ip configuration file:
	 *
	 * HWADDR=macaddr
	 * IF_NAME=interface name
	 * DHCP=yes (This is optional; if yes, DHCP is configured)
	 *
	 * IPADDR=ipaddr1
	 * IPADDR_1=ipaddr2
	 * IPADDR_x=ipaddry (where y = x + 1)
	 *
	 * NETMASK=netmask1
	 * NETMASK_x=netmasky (where y = x + 1)
	 *
	 * GATEWAY=ipaddr1
	 * GATEWAY_x=ipaddry (where y = x + 1)
	 *
	 * DNSx=ipaddrx (where first DNS address is tagged as DNS1 etc)
	 *
	 * IPV6 addresses will be tagged as IPV6ADDR, IPV6 gateway will be
	 * tagged as IPV6_DEFAULTGW and IPV6 NETMASK will be tagged as
	 * IPV6NETMASK.
	 *
	 * The host can specify multiple ipv4 and ipv6 addresses to be
	 * configured for the interface. Furthermore, the configuration
	 * needs to be persistent. A subsequent GET call on the interface
	 * is expected to return the configuration that is set via the SET
	 * call.
	 */

	snprintf(if_file, sizeof(if_file), "%s%s%s", KVP_CONFIG_LOC,
		"hyperv/ifcfg-", if_name);

	file = fopen(if_file, "w");

	if (file == NULL) {
		syslog(LOG_ERR, "Failed to open config file");
		return HV_E_FAIL;
	}

	/*
	 * First write out the MAC address.
	 */

	mac_addr = kvp_if_name_to_mac(if_name);
	if (mac_addr == NULL) {
		error = HV_E_FAIL;
		goto setval_error;
	}

	error = kvp_write_file(file, "HWADDR", "", mac_addr);
	if (error)
		goto setval_error;

	error = kvp_write_file(file, "IF_NAME", "", if_name);
	if (error)
		goto setval_error;

	if (new_val->dhcp_enabled) {
		error = kvp_write_file(file, "DHCP", "", "yes");
		if (error)
			goto setval_error;

		/*
		 * We are done!.
		 */
		goto setval_done;
	}

	/*
	 * Write the configuration for ipaddress, netmask, gateway and
	 * name servers.
	 */

	error = process_ip_string(file, (char *)new_val->ip_addr, IPADDR);
	if (error)
		goto setval_error;

	error = process_ip_string(file, (char *)new_val->sub_net, NETMASK);
	if (error)
		goto setval_error;

	error = process_ip_string(file, (char *)new_val->gate_way, GATEWAY);
	if (error)
		goto setval_error;

	error = process_ip_string(file, (char *)new_val->dns_addr, DNS);
	if (error)
		goto setval_error;

setval_done:
	free(mac_addr);
	fclose(file);

	/*
	 * Now that we have populated the configuration file,
	 * invoke the external script to do its magic.
	 */

	snprintf(cmd, sizeof(cmd), "%s %s", "hv_set_ifconfig", if_file);
	system(cmd);
	return 0;

setval_error:
	syslog(LOG_ERR, "Failed to write config file");
	free(mac_addr);
	fclose(file);
	return error;
}