PHP_METHOD(Phar, addEmptyDir)
{
	char *dirname;
	int dirname_len;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dirname, &dirname_len) == FAILURE) {
		return;
	}

	if (dirname_len >= sizeof(".phar")-1 && !memcmp(dirname, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot create a directory in magic \".phar\" directory");
		return;
	}

	phar_mkdir(&phar_obj->arc.archive, dirname, dirname_len TSRMLS_CC);
}