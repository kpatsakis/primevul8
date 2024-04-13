ipmi_set_user_access(struct ipmi_intf *intf, int argc, char **argv)
{
	struct user_access_t user_access = {0};
	int ccode = 0;
	int i = 0;
	uint8_t channel = 0;
	uint8_t priv = 0;
	uint8_t user_id = 0;
	if (argc > 0 && strncmp(argv[0], "help", 4) == 0) {
		printf_channel_usage();
		return 0;
	} else if (argc < 3) {
		lprintf(LOG_ERR, "Not enough parameters given.");
		printf_channel_usage();
		return (-1);
	}
	if (is_ipmi_channel_num(argv[0], &channel) != 0
			|| is_ipmi_user_id(argv[1], &user_id) != 0) {
		return (-1);
	}
	user_access.channel = channel;
	user_access.user_id = user_id;
	ccode = _ipmi_get_user_access(intf, &user_access);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Get User Access (channel %d id %d)",
				channel, user_id);
		return (-1);
	}
	for (i = 2; i < argc; i ++) {
		if (strncmp(argv[i], "callin=", 7) == 0) {
			if (strncmp(argv[i] + 7, "off", 3) == 0) {
				user_access.callin_callback = 1;
			} else {
				user_access.callin_callback = 0;
			}
		} else if (strncmp(argv[i], "link=", 5) == 0) {
			if (strncmp(argv[i] + 5, "off", 3) == 0) {
				user_access.link_auth = 0;
			} else {
				user_access.link_auth = 1;
			}
		} else if (strncmp(argv[i], "ipmi=", 5) == 0) {
			if (strncmp(argv[i] + 5, "off", 3) == 0) {
				user_access.ipmi_messaging = 0;
			} else {
				user_access.ipmi_messaging = 1;
			}
		} else if (strncmp(argv[i], "privilege=", 10) == 0) {
			if (str2uchar(argv[i] + 10, &priv) != 0) {
				lprintf(LOG_ERR,
						"Numeric value expected, but '%s' given.",
						argv[i] + 10);
				return (-1);
			}
			user_access.privilege_limit = priv;
		} else {
			lprintf(LOG_ERR, "Invalid option: %s\n", argv[i]);
			return (-1);
		}
	}
	ccode = _ipmi_set_user_access(intf, &user_access, 0);
	if (eval_ccode(ccode) != 0) {
		lprintf(LOG_ERR,
				"Unable to Set User Access (channel %d id %d)",
				channel, user_id);
		return (-1);
	}
	printf("Set User Access (channel %d id %d) successful.\n",
			channel, user_id);
	return 0;
}