PHP_FUNCTION(escapeshellarg)
{
	char *argument;
	int argument_len;
	char *cmd = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &argument, &argument_len) == FAILURE) {
		return;
	}

	if (argument) {
		RETVAL_STR(php_escape_shell_arg(argument));
	}
}