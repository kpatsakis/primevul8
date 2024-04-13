void mwifiex_set_sys_config_invalid_data(struct mwifiex_uap_bss_param *config)
{
	config->bcast_ssid_ctl = 0x7F;
	config->radio_ctl = 0x7F;
	config->dtim_period = 0x7F;
	config->beacon_period = 0x7FFF;
	config->auth_mode = 0x7F;
	config->rts_threshold = 0x7FFF;
	config->frag_threshold = 0x7FFF;
	config->retry_limit = 0x7F;
	config->qos_info = 0xFF;
}