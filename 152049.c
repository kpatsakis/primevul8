static void xml_parser_dtor(zend_resource *rsrc)
{
	xml_parser *parser = (xml_parser *)rsrc->ptr;
	
	if (parser->parser) {
		XML_ParserFree(parser->parser);
	}
	if (parser->ltags) {
		int inx;
		for (inx = 0; ((inx < parser->level) && (inx < XML_MAXLEVEL)); inx++)
			efree(parser->ltags[ inx ]);
		efree(parser->ltags);
	}
	if (!Z_ISUNDEF(parser->startElementHandler)) {
		zval_ptr_dtor(&parser->startElementHandler);
	}
	if (!Z_ISUNDEF(parser->endElementHandler)) {
		zval_ptr_dtor(&parser->endElementHandler);
	}
	if (!Z_ISUNDEF(parser->characterDataHandler)) {
		zval_ptr_dtor(&parser->characterDataHandler);
	}
	if (!Z_ISUNDEF(parser->processingInstructionHandler)) {
		zval_ptr_dtor(&parser->processingInstructionHandler);
	}
	if (!Z_ISUNDEF(parser->defaultHandler)) {
		zval_ptr_dtor(&parser->defaultHandler);
	}
	if (!Z_ISUNDEF(parser->unparsedEntityDeclHandler)) {
		zval_ptr_dtor(&parser->unparsedEntityDeclHandler);
	}
	if (!Z_ISUNDEF(parser->notationDeclHandler)) {
		zval_ptr_dtor(&parser->notationDeclHandler);
	}
	if (!Z_ISUNDEF(parser->externalEntityRefHandler)) {
		zval_ptr_dtor(&parser->externalEntityRefHandler);
	}
	if (!Z_ISUNDEF(parser->unknownEncodingHandler)) {
		zval_ptr_dtor(&parser->unknownEncodingHandler);
	}
	if (!Z_ISUNDEF(parser->startNamespaceDeclHandler)) {
		zval_ptr_dtor(&parser->startNamespaceDeclHandler);
	}
	if (!Z_ISUNDEF(parser->endNamespaceDeclHandler)) {
		zval_ptr_dtor(&parser->endNamespaceDeclHandler);
	}
	if (parser->baseURI) {
		efree(parser->baseURI);
	}
	if (!Z_ISUNDEF(parser->object)) {
		zval_ptr_dtor(&parser->object);
	}

	efree(parser);
}