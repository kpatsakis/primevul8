ipmi_channel_main(struct ipmi_intf *intf, int argc, char **argv)
{
	int retval = 0;
	uint8_t channel;
	uint8_t priv = 0;
	if (argc < 1) {
		lprintf(LOG_ERR, "Not enough parameters given.");
		printf_channel_usage();
		return (-1);
	} else if (strncmp(argv[0], "help", 4) == 0) {
		printf_channel_usage();
		return 0;
	} else if (strncmp(argv[0], "authcap", 7) == 0) {
		if (argc != 3) {
			printf_channel_usage();
			return (-1);
		}
		if (is_ipmi_channel_num(argv[1], &channel) != 0
				|| is_ipmi_user_priv_limit(argv[2], &priv) != 0) {
			return (-1);
		}
		retval = ipmi_get_channel_auth_cap(intf, channel, priv);
	} else if (strncmp(argv[0], "getaccess", 10) == 0) {
		uint8_t user_id = 0;
		if ((argc < 2) || (argc > 3)) {
			lprintf(LOG_ERR, "Not enough parameters given.");
			printf_channel_usage();
			return (-1);
		}
		if (is_ipmi_channel_num(argv[1], &channel) != 0) {
			return (-1);
		}
		if (argc == 3) {
			if (is_ipmi_user_id(argv[2], &user_id) != 0) {
				return (-1);
			}
		}
		retval = ipmi_get_user_access(intf, channel, user_id);
	} else if (strncmp(argv[0], "setaccess", 9) == 0) {
		return ipmi_set_user_access(intf, (argc - 1), &(argv[1]));
	} else if (strncmp(argv[0], "info", 4) == 0) {
		channel = 0xE;
		if (argc > 2) {
			printf_channel_usage();
			return (-1);
		}
		if (argc == 2) {
			if (is_ipmi_channel_num(argv[1], &channel) != 0) {
				return (-1);
			}
		}
		retval = ipmi_get_channel_info(intf, channel);
	} else if (strncmp(argv[0], "getciphers", 10) == 0) {
		/* channel getciphers <ipmi|sol> [channel] */
		channel = 0xE;
		if ((argc < 2) || (argc > 3) ||
		    (strncmp(argv[1], "ipmi", 4) && strncmp(argv[1], "sol",  3))) {
			printf_channel_usage();
			return (-1);
		}
		if (argc == 3) {
			if (is_ipmi_channel_num(argv[2], &channel) != 0) {
				return (-1);
			}
		}
		retval = ipmi_print_channel_cipher_suites(intf,
		                                          argv[1], /* ipmi | sol */
		                                          channel);
	} else {
		lprintf(LOG_ERR, "Invalid CHANNEL command: %s\n", argv[0]);
		printf_channel_usage();
		retval = -1;
	}
	return retval;
}