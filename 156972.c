PHPAPI zend_string *php_escape_shell_cmd(char *str)
{
	register int x, y, l = strlen(str);
	char *p = NULL;
	size_t estimate = (2 * l) + 1;
	zend_string *cmd;

	TSRMLS_FETCH();

	cmd = STR_ALLOC(2 * l, 0);

	for (x = 0, y = 0; x < l; x++) {
		int mb_len = php_mblen(str + x, (l - x));

		/* skip non-valid multibyte characters */
		if (mb_len < 0) {
			continue;
		} else if (mb_len > 1) {
			memcpy(cmd->val + y, str + x, mb_len);
			y += mb_len;
			x += mb_len - 1;
			continue;
		}

		switch (str[x]) {
#ifndef PHP_WIN32
			case '"':
			case '\'':
				if (!p && (p = memchr(str + x + 1, str[x], l - x - 1))) {
					/* noop */
				} else if (p && *p == str[x]) {
					p = NULL;
				} else {
					cmd->val[y++] = '\\';
				}
				cmd->val[y++] = str[x];
				break;
#else
			/* % is Windows specific for enviromental variables, ^%PATH% will 
				output PATH whil ^%PATH^% not. escapeshellcmd->val will escape all %.
			*/
			case '%':
			case '"':
			case '\'':
#endif
			case '#': /* This is character-set independent */
			case '&':
			case ';':
			case '`':
			case '|':
			case '*':
			case '?':
			case '~':
			case '<':
			case '>':
			case '^':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '$':
			case '\\':
			case '\x0A': /* excluding these two */
			case '\xFF':
#ifdef PHP_WIN32
				cmd->val[y++] = '^';
#else
				cmd->val[y++] = '\\';
#endif
				/* fall-through */
			default:
				cmd->val[y++] = str[x];

		}
	}
	cmd->val[y] = '\0';

	if ((estimate - y) > 4096) {
		/* realloc if the estimate was way overill
		 * Arbitrary cutoff point of 4096 */
		cmd = STR_REALLOC(cmd, y, 0);
	}

	cmd->len = y;

	return cmd;
}