parserInit(XML_Parser parser, const XML_Char *encodingName) {
  parser->m_processor = prologInitProcessor;
  XmlPrologStateInit(&parser->m_prologState);
  if (encodingName != NULL) {
    parser->m_protocolEncodingName = copyString(encodingName, &(parser->m_mem));
  }
  parser->m_curBase = NULL;
  XmlInitEncoding(&parser->m_initEncoding, &parser->m_encoding, 0);
  parser->m_userData = NULL;
  parser->m_handlerArg = NULL;
  parser->m_startElementHandler = NULL;
  parser->m_endElementHandler = NULL;
  parser->m_characterDataHandler = NULL;
  parser->m_processingInstructionHandler = NULL;
  parser->m_commentHandler = NULL;
  parser->m_startCdataSectionHandler = NULL;
  parser->m_endCdataSectionHandler = NULL;
  parser->m_defaultHandler = NULL;
  parser->m_startDoctypeDeclHandler = NULL;
  parser->m_endDoctypeDeclHandler = NULL;
  parser->m_unparsedEntityDeclHandler = NULL;
  parser->m_notationDeclHandler = NULL;
  parser->m_startNamespaceDeclHandler = NULL;
  parser->m_endNamespaceDeclHandler = NULL;
  parser->m_notStandaloneHandler = NULL;
  parser->m_externalEntityRefHandler = NULL;
  parser->m_externalEntityRefHandlerArg = parser;
  parser->m_skippedEntityHandler = NULL;
  parser->m_elementDeclHandler = NULL;
  parser->m_attlistDeclHandler = NULL;
  parser->m_entityDeclHandler = NULL;
  parser->m_xmlDeclHandler = NULL;
  parser->m_bufferPtr = parser->m_buffer;
  parser->m_bufferEnd = parser->m_buffer;
  parser->m_parseEndByteIndex = 0;
  parser->m_parseEndPtr = NULL;
  parser->m_declElementType = NULL;
  parser->m_declAttributeId = NULL;
  parser->m_declEntity = NULL;
  parser->m_doctypeName = NULL;
  parser->m_doctypeSysid = NULL;
  parser->m_doctypePubid = NULL;
  parser->m_declAttributeType = NULL;
  parser->m_declNotationName = NULL;
  parser->m_declNotationPublicId = NULL;
  parser->m_declAttributeIsCdata = XML_FALSE;
  parser->m_declAttributeIsId = XML_FALSE;
  memset(&parser->m_position, 0, sizeof(POSITION));
  parser->m_errorCode = XML_ERROR_NONE;
  parser->m_eventPtr = NULL;
  parser->m_eventEndPtr = NULL;
  parser->m_positionPtr = NULL;
  parser->m_openInternalEntities = NULL;
  parser->m_defaultExpandInternalEntities = XML_TRUE;
  parser->m_tagLevel = 0;
  parser->m_tagStack = NULL;
  parser->m_inheritedBindings = NULL;
  parser->m_nSpecifiedAtts = 0;
  parser->m_unknownEncodingMem = NULL;
  parser->m_unknownEncodingRelease = NULL;
  parser->m_unknownEncodingData = NULL;
  parser->m_parentParser = NULL;
  parser->m_parsingStatus.parsing = XML_INITIALIZED;
#ifdef XML_DTD
  parser->m_isParamEntity = XML_FALSE;
  parser->m_useForeignDTD = XML_FALSE;
  parser->m_paramEntityParsing = XML_PARAM_ENTITY_PARSING_NEVER;
#endif
  parser->m_hash_secret_salt = 0;

#ifdef XML_DTD
  memset(&parser->m_accounting, 0, sizeof(ACCOUNTING));
  parser->m_accounting.debugLevel = getDebugLevel("EXPAT_ACCOUNTING_DEBUG", 0u);
  parser->m_accounting.maximumAmplificationFactor
      = EXPAT_BILLION_LAUGHS_ATTACK_PROTECTION_MAXIMUM_AMPLIFICATION_DEFAULT;
  parser->m_accounting.activationThresholdBytes
      = EXPAT_BILLION_LAUGHS_ATTACK_PROTECTION_ACTIVATION_THRESHOLD_DEFAULT;

  memset(&parser->m_entity_stats, 0, sizeof(ENTITY_STATS));
  parser->m_entity_stats.debugLevel = getDebugLevel("EXPAT_ENTITY_DEBUG", 0u);
#endif
}