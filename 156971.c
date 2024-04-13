PHPAPI int php_exec(int type, char *cmd, zval *array, zval *return_value TSRMLS_DC)
{
	FILE *fp;
	char *buf;
	int l = 0, pclose_return;
	char *b, *d=NULL;
	php_stream *stream;
	size_t buflen, bufl = 0;
#if PHP_SIGCHILD
	void (*sig_handler)() = NULL;
#endif

#if PHP_SIGCHILD
	sig_handler = signal (SIGCHLD, SIG_DFL);
#endif

#ifdef PHP_WIN32
	fp = VCWD_POPEN(cmd, "rb");
#else
	fp = VCWD_POPEN(cmd, "r");
#endif
	if (!fp) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", cmd);
		goto err;
	}

	stream = php_stream_fopen_from_pipe(fp, "rb");

	buf = (char *) emalloc(EXEC_INPUT_BUF);
	buflen = EXEC_INPUT_BUF;

	if (type != 3) {
		b = buf;
		
		while (php_stream_get_line(stream, b, EXEC_INPUT_BUF, &bufl)) {
			/* no new line found, let's read some more */
			if (b[bufl - 1] != '\n' && !php_stream_eof(stream)) {
				if (buflen < (bufl + (b - buf) + EXEC_INPUT_BUF)) {
					bufl += b - buf;
					buflen = bufl + EXEC_INPUT_BUF;
					buf = erealloc(buf, buflen);
					b = buf + bufl;
				} else {
					b += bufl;
				}
				continue;
			} else if (b != buf) {
				bufl += b - buf;
			}

			if (type == 1) {
				PHPWRITE(buf, bufl);
				if (php_output_get_level(TSRMLS_C) < 1) {
					sapi_flush(TSRMLS_C);
				}
			} else if (type == 2) {
				/* strip trailing whitespaces */
				l = bufl;
				while (l-- && isspace(((unsigned char *)buf)[l]));
				if (l != (int)(bufl - 1)) {
					bufl = l + 1;
					buf[bufl] = '\0';
				}
				add_next_index_stringl(array, buf, bufl, 1);
			}
			b = buf;
		}
		if (bufl) {
			/* strip trailing whitespaces if we have not done so already */
			if ((type == 2 && buf != b) || type != 2) {
				l = bufl;
				while (l-- && isspace(((unsigned char *)buf)[l]));
				if (l != (int)(bufl - 1)) {
					bufl = l + 1;
					buf[bufl] = '\0';
				}
				if (type == 2) {
					add_next_index_stringl(array, buf, bufl, 1);
				}
			}

			/* Return last line from the shell command */
			RETVAL_STRINGL(buf, bufl);
		} else { /* should return NULL, but for BC we return "" */
			RETVAL_EMPTY_STRING();
		}
	} else {
		while((bufl = php_stream_read(stream, buf, EXEC_INPUT_BUF)) > 0) {
			PHPWRITE(buf, bufl);
		}
	}

	pclose_return = php_stream_close(stream);
	efree(buf);

done:
#if PHP_SIGCHILD
	if (sig_handler) {
		signal(SIGCHLD, sig_handler);
	}
#endif
	if (d) {
		efree(d);
	}
	return pclose_return;
err:
	pclose_return = -1;
	goto done;
}