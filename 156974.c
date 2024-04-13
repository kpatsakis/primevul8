PHP_FUNCTION(escapeshellcmd)
{
	char *command;
	int command_len;
	char *cmd = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &command, &command_len) == FAILURE) {
		return;
	}

	if (command_len) {
		RETVAL_STRING(php_escape_shell_cmd(command));
	} else {
		RETVAL_EMPTY_STRING();
	}
}