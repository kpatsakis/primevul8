PHP_FUNCTION(xml_set_object)
{
	xml_parser *parser;
	zval *pind, *mythis;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ro/", &pind, &mythis) == FAILURE) {
		return;
	}

	if ((parser = (xml_parser *)zend_fetch_resource(Z_RES_P(pind), "XML Parser", le_xml_parser)) == NULL) {
		RETURN_FALSE;
	}

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
	if (!Z_ISUNDEF(parser->object)) {
		zval_ptr_dtor(&parser->object);
	}

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
/* #ifdef ZEND_ENGINE_2
	zval_add_ref(&parser->object); 
#endif */

	ZVAL_COPY(&parser->object, mythis);

	RETVAL_TRUE;
}