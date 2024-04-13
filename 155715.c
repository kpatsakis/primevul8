XML_ExternalEntityParserCreate(XML_Parser oldParser, const XML_Char *context,
                               const XML_Char *encodingName) {
  XML_Parser parser = oldParser;
  DTD *newDtd = NULL;
  DTD *oldDtd;
  XML_StartElementHandler oldStartElementHandler;
  XML_EndElementHandler oldEndElementHandler;
  XML_CharacterDataHandler oldCharacterDataHandler;
  XML_ProcessingInstructionHandler oldProcessingInstructionHandler;
  XML_CommentHandler oldCommentHandler;
  XML_StartCdataSectionHandler oldStartCdataSectionHandler;
  XML_EndCdataSectionHandler oldEndCdataSectionHandler;
  XML_DefaultHandler oldDefaultHandler;
  XML_UnparsedEntityDeclHandler oldUnparsedEntityDeclHandler;
  XML_NotationDeclHandler oldNotationDeclHandler;
  XML_StartNamespaceDeclHandler oldStartNamespaceDeclHandler;
  XML_EndNamespaceDeclHandler oldEndNamespaceDeclHandler;
  XML_NotStandaloneHandler oldNotStandaloneHandler;
  XML_ExternalEntityRefHandler oldExternalEntityRefHandler;
  XML_SkippedEntityHandler oldSkippedEntityHandler;
  XML_UnknownEncodingHandler oldUnknownEncodingHandler;
  XML_ElementDeclHandler oldElementDeclHandler;
  XML_AttlistDeclHandler oldAttlistDeclHandler;
  XML_EntityDeclHandler oldEntityDeclHandler;
  XML_XmlDeclHandler oldXmlDeclHandler;
  ELEMENT_TYPE *oldDeclElementType;

  void *oldUserData;
  void *oldHandlerArg;
  XML_Bool oldDefaultExpandInternalEntities;
  XML_Parser oldExternalEntityRefHandlerArg;
#ifdef XML_DTD
  enum XML_ParamEntityParsing oldParamEntityParsing;
  int oldInEntityValue;
#endif
  XML_Bool oldns_triplets;
  /* Note that the new parser shares the same hash secret as the old
     parser, so that dtdCopy and copyEntityTable can lookup values
     from hash tables associated with either parser without us having
     to worry which hash secrets each table has.
  */
  unsigned long oldhash_secret_salt;

  /* Validate the oldParser parameter before we pull everything out of it */
  if (oldParser == NULL)
    return NULL;

  /* Stash the original parser contents on the stack */
  oldDtd = parser->m_dtd;
  oldStartElementHandler = parser->m_startElementHandler;
  oldEndElementHandler = parser->m_endElementHandler;
  oldCharacterDataHandler = parser->m_characterDataHandler;
  oldProcessingInstructionHandler = parser->m_processingInstructionHandler;
  oldCommentHandler = parser->m_commentHandler;
  oldStartCdataSectionHandler = parser->m_startCdataSectionHandler;
  oldEndCdataSectionHandler = parser->m_endCdataSectionHandler;
  oldDefaultHandler = parser->m_defaultHandler;
  oldUnparsedEntityDeclHandler = parser->m_unparsedEntityDeclHandler;
  oldNotationDeclHandler = parser->m_notationDeclHandler;
  oldStartNamespaceDeclHandler = parser->m_startNamespaceDeclHandler;
  oldEndNamespaceDeclHandler = parser->m_endNamespaceDeclHandler;
  oldNotStandaloneHandler = parser->m_notStandaloneHandler;
  oldExternalEntityRefHandler = parser->m_externalEntityRefHandler;
  oldSkippedEntityHandler = parser->m_skippedEntityHandler;
  oldUnknownEncodingHandler = parser->m_unknownEncodingHandler;
  oldElementDeclHandler = parser->m_elementDeclHandler;
  oldAttlistDeclHandler = parser->m_attlistDeclHandler;
  oldEntityDeclHandler = parser->m_entityDeclHandler;
  oldXmlDeclHandler = parser->m_xmlDeclHandler;
  oldDeclElementType = parser->m_declElementType;

  oldUserData = parser->m_userData;
  oldHandlerArg = parser->m_handlerArg;
  oldDefaultExpandInternalEntities = parser->m_defaultExpandInternalEntities;
  oldExternalEntityRefHandlerArg = parser->m_externalEntityRefHandlerArg;
#ifdef XML_DTD
  oldParamEntityParsing = parser->m_paramEntityParsing;
  oldInEntityValue = parser->m_prologState.inEntityValue;
#endif
  oldns_triplets = parser->m_ns_triplets;
  /* Note that the new parser shares the same hash secret as the old
     parser, so that dtdCopy and copyEntityTable can lookup values
     from hash tables associated with either parser without us having
     to worry which hash secrets each table has.
  */
  oldhash_secret_salt = parser->m_hash_secret_salt;

#ifdef XML_DTD
  if (! context)
    newDtd = oldDtd;
#endif /* XML_DTD */

  /* Note that the magical uses of the pre-processor to make field
     access look more like C++ require that `parser' be overwritten
     here.  This makes this function more painful to follow than it
     would be otherwise.
  */
  if (parser->m_ns) {
    XML_Char tmp[2];
    *tmp = parser->m_namespaceSeparator;
    parser = parserCreate(encodingName, &parser->m_mem, tmp, newDtd);
  } else {
    parser = parserCreate(encodingName, &parser->m_mem, NULL, newDtd);
  }

  if (! parser)
    return NULL;

  parser->m_startElementHandler = oldStartElementHandler;
  parser->m_endElementHandler = oldEndElementHandler;
  parser->m_characterDataHandler = oldCharacterDataHandler;
  parser->m_processingInstructionHandler = oldProcessingInstructionHandler;
  parser->m_commentHandler = oldCommentHandler;
  parser->m_startCdataSectionHandler = oldStartCdataSectionHandler;
  parser->m_endCdataSectionHandler = oldEndCdataSectionHandler;
  parser->m_defaultHandler = oldDefaultHandler;
  parser->m_unparsedEntityDeclHandler = oldUnparsedEntityDeclHandler;
  parser->m_notationDeclHandler = oldNotationDeclHandler;
  parser->m_startNamespaceDeclHandler = oldStartNamespaceDeclHandler;
  parser->m_endNamespaceDeclHandler = oldEndNamespaceDeclHandler;
  parser->m_notStandaloneHandler = oldNotStandaloneHandler;
  parser->m_externalEntityRefHandler = oldExternalEntityRefHandler;
  parser->m_skippedEntityHandler = oldSkippedEntityHandler;
  parser->m_unknownEncodingHandler = oldUnknownEncodingHandler;
  parser->m_elementDeclHandler = oldElementDeclHandler;
  parser->m_attlistDeclHandler = oldAttlistDeclHandler;
  parser->m_entityDeclHandler = oldEntityDeclHandler;
  parser->m_xmlDeclHandler = oldXmlDeclHandler;
  parser->m_declElementType = oldDeclElementType;
  parser->m_userData = oldUserData;
  if (oldUserData == oldHandlerArg)
    parser->m_handlerArg = parser->m_userData;
  else
    parser->m_handlerArg = parser;
  if (oldExternalEntityRefHandlerArg != oldParser)
    parser->m_externalEntityRefHandlerArg = oldExternalEntityRefHandlerArg;
  parser->m_defaultExpandInternalEntities = oldDefaultExpandInternalEntities;
  parser->m_ns_triplets = oldns_triplets;
  parser->m_hash_secret_salt = oldhash_secret_salt;
  parser->m_parentParser = oldParser;
#ifdef XML_DTD
  parser->m_paramEntityParsing = oldParamEntityParsing;
  parser->m_prologState.inEntityValue = oldInEntityValue;
  if (context) {
#endif /* XML_DTD */
    if (! dtdCopy(oldParser, parser->m_dtd, oldDtd, &parser->m_mem)
        || ! setContext(parser, context)) {
      XML_ParserFree(parser);
      return NULL;
    }
    parser->m_processor = externalEntityInitProcessor;
#ifdef XML_DTD
  } else {
    /* The DTD instance referenced by parser->m_dtd is shared between the
       document's root parser and external PE parsers, therefore one does not
       need to call setContext. In addition, one also *must* not call
       setContext, because this would overwrite existing prefix->binding
       pointers in parser->m_dtd with ones that get destroyed with the external
       PE parser. This would leave those prefixes with dangling pointers.
    */
    parser->m_isParamEntity = XML_TRUE;
    XmlPrologStateInitExternalEntity(&parser->m_prologState);
    parser->m_processor = externalParEntInitProcessor;
  }
#endif /* XML_DTD */
  return parser;
}