static inline size_t write_octet_sequence(unsigned char *buf, enum entity_charset charset, unsigned code) {
	/* code is not necessarily a unicode code point */
	switch (charset) {
	case cs_utf_8:
		return php_utf32_utf8(buf, code);

	case cs_8859_1:
	case cs_cp1252:
	case cs_8859_15:
	case cs_koi8r:
	case cs_cp1251:
	case cs_8859_5:
	case cs_cp866:
	case cs_macroman:
		/* single byte stuff */
		*buf = code;
		return 1;

	case cs_big5:
	case cs_big5hkscs:
	case cs_sjis:
	case cs_gb2312:
		/* we don't have complete unicode mappings for these yet in entity_decode,
		 * and we opt to pass through the octet sequences for these in htmlentities
		 * instead of converting to an int and then converting back. */
#if 0
		return php_mb2_int_to_char(buf, code);
#else
#ifdef ZEND_DEBUG
		assert(code <= 0xFFU);
#endif
		*buf = code;
		return 1;
#endif

	case cs_eucjp:
#if 0 /* idem */
		return php_mb2_int_to_char(buf, code);
#else
#ifdef ZEND_DEBUG
		assert(code <= 0xFFU);
#endif
		*buf = code;
		return 1;
#endif

	default:
		assert(0);
		return 0;
	}
}