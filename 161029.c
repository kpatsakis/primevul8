ipmi_get_user_access(struct ipmi_intf *intf, uint8_t channel, uint8_t user_id)
{
	struct user_access_t user_access;
	struct user_name_t user_name;
	int ccode = 0;
	int curr_uid;
	int init = 1;
	int max_uid = 0;

	curr_uid = user_id ? user_id : 1;
	do {
		memset(&user_access, 0, sizeof(user_access));
		user_access.channel = channel;
		user_access.user_id = curr_uid;
		ccode = _ipmi_get_user_access(intf, &user_access);
		if (eval_ccode(ccode) != 0) {
			lprintf(LOG_ERR,
					"Unable to Get User Access (channel %d id %d)",
					channel, curr_uid);
			return (-1);
		}

		memset(&user_name, 0, sizeof(user_name));
		user_name.user_id = curr_uid;
		ccode = _ipmi_get_user_name(intf, &user_name);
		if (ccode == 0xCC) {
			user_name.user_id = curr_uid;
			memset(&user_name.user_name, '\0', 17);
		} else if (eval_ccode(ccode) != 0) {
			lprintf(LOG_ERR, "Unable to Get User Name (id %d)", curr_uid);
			return (-1);
		}
		if (init) {
			printf("Maximum User IDs     : %d\n", user_access.max_user_ids);
			printf("Enabled User IDs     : %d\n", user_access.enabled_user_ids);
			max_uid = user_access.max_user_ids;
			init = 0;
		}

		printf("\n");
		printf("User ID              : %d\n", curr_uid);
		printf("User Name            : %s\n", user_name.user_name);
		printf("Fixed Name           : %s\n",
		       (curr_uid <= user_access.fixed_user_ids) ? "Yes" : "No");
		printf("Access Available     : %s\n",
		       (user_access.callin_callback) ? "callback" : "call-in / callback");
		printf("Link Authentication  : %sabled\n",
		       (user_access.link_auth) ? "en" : "dis");
		printf("IPMI Messaging       : %sabled\n",
		       (user_access.ipmi_messaging) ? "en" : "dis");
		printf("Privilege Level      : %s\n",
		       val2str(user_access.privilege_limit, ipmi_privlvl_vals));
		printf("Enable Status        : %s\n",
			val2str(user_access.enable_status, ipmi_user_enable_status_vals));
		curr_uid ++;
	} while (!user_id && curr_uid <= max_uid);

	return 0;
}