PHP_FUNCTION(xml_parser_set_option)
{
	xml_parser *parser;
	zval *pind, *val;
	zend_long opt;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rlz", &pind, &opt, &val) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	switch (opt) {
		case PHP_XML_OPTION_CASE_FOLDING:
			convert_to_long_ex(val);
			parser->case_folding = Z_LVAL_P(val);
			break;
		case PHP_XML_OPTION_SKIP_TAGSTART:
			convert_to_long_ex(val);
			parser->toffset = Z_LVAL_P(val);
			break;
		case PHP_XML_OPTION_SKIP_WHITE:
			convert_to_long_ex(val);
			parser->skipwhite = Z_LVAL_P(val);
			break;
		case PHP_XML_OPTION_TARGET_ENCODING: {
			xml_encoding *enc;
			convert_to_string_ex(val);
			enc = xml_get_encoding((XML_Char*)Z_STRVAL_P(val));
			if (enc == NULL) {
				php_error_docref(NULL, E_WARNING, "Unsupported target encoding \"%s\"", Z_STRVAL_P(val));
				RETURN_FALSE;
			}
			parser->target_encoding = enc->name;
			break;
		}
		default:
			php_error_docref(NULL, E_WARNING, "Unknown option");
			RETURN_FALSE;
			break;
	}
	RETVAL_TRUE;
}