kvp_get_ip_info(int family, char *if_name, int op,
		 void  *out_buffer, int length)
{
	struct ifaddrs *ifap;
	struct ifaddrs *curp;
	int offset = 0;
	int sn_offset = 0;
	int error = 0;
	char *buffer;
	struct hv_kvp_ipaddr_value *ip_buffer;
	char cidr_mask[5]; /* /xyz */
	int weight;
	int i;
	unsigned int *w;
	char *sn_str;
	struct sockaddr_in6 *addr6;

	if (op == KVP_OP_ENUMERATE) {
		buffer = out_buffer;
	} else {
		ip_buffer = out_buffer;
		buffer = (char *)ip_buffer->ip_addr;
		ip_buffer->addr_family = 0;
	}
	/*
	 * On entry into this function, the buffer is capable of holding the
	 * maximum key value.
	 */

	if (getifaddrs(&ifap)) {
		strcpy(buffer, "getifaddrs failed\n");
		return HV_E_FAIL;
	}

	curp = ifap;
	while (curp != NULL) {
		if (curp->ifa_addr == NULL) {
			curp = curp->ifa_next;
			continue;
		}

		if ((if_name != NULL) &&
			(strncmp(curp->ifa_name, if_name, strlen(if_name)))) {
			/*
			 * We want info about a specific interface;
			 * just continue.
			 */
			curp = curp->ifa_next;
			continue;
		}

		/*
		 * We only support two address families: AF_INET and AF_INET6.
		 * If a family value of 0 is specified, we collect both
		 * supported address families; if not we gather info on
		 * the specified address family.
		 */
		if ((((family != 0) &&
			 (curp->ifa_addr->sa_family != family))) ||
			 (curp->ifa_flags & IFF_LOOPBACK)) {
			curp = curp->ifa_next;
			continue;
		}
		if ((curp->ifa_addr->sa_family != AF_INET) &&
			(curp->ifa_addr->sa_family != AF_INET6)) {
			curp = curp->ifa_next;
			continue;
		}

		if (op == KVP_OP_GET_IP_INFO) {
			/*
			 * Gather info other than the IP address.
			 * IP address info will be gathered later.
			 */
			if (curp->ifa_addr->sa_family == AF_INET) {
				ip_buffer->addr_family |= ADDR_FAMILY_IPV4;
				/*
				 * Get subnet info.
				 */
				error = kvp_process_ip_address(
							     curp->ifa_netmask,
							     AF_INET,
							     (char *)
							     ip_buffer->sub_net,
							     length,
							     &sn_offset);
				if (error)
					goto gather_ipaddr;
			} else {
				ip_buffer->addr_family |= ADDR_FAMILY_IPV6;

				/*
				 * Get subnet info in CIDR format.
				 */
				weight = 0;
				sn_str = (char *)ip_buffer->sub_net;
				addr6 = (struct sockaddr_in6 *)
					curp->ifa_netmask;
				w = addr6->sin6_addr.s6_addr32;

				for (i = 0; i < 4; i++)
					weight += hweight32(&w[i]);

				sprintf(cidr_mask, "/%d", weight);
				if ((length - sn_offset) <
					(strlen(cidr_mask) + 1))
					goto gather_ipaddr;

				if (sn_offset == 0)
					strcpy(sn_str, cidr_mask);
				else
					strcat(sn_str, cidr_mask);
				strcat((char *)ip_buffer->sub_net, ";");
				sn_offset += strlen(sn_str) + 1;
			}

			/*
			 * Collect other ip related configuration info.
			 */

			kvp_get_ipconfig_info(if_name, ip_buffer);
		}

gather_ipaddr:
		error = kvp_process_ip_address(curp->ifa_addr,
						curp->ifa_addr->sa_family,
						buffer,
						length, &offset);
		if (error)
			goto getaddr_done;

		curp = curp->ifa_next;
	}

getaddr_done:
	freeifaddrs(ifap);
	return error;
}