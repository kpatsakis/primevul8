static int val_hdr(struct arg *arg, char **err_msg)
{
	if (arg && arg[1].type == ARGT_SINT && arg[1].data.sint < -MAX_HDR_HISTORY) {
		memprintf(err_msg, "header occurrence must be >= %d", -MAX_HDR_HISTORY);
		return 0;
	}
	return 1;
}