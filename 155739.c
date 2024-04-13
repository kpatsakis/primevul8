XML_ParserReset(XML_Parser parser, const XML_Char *encodingName) {
  TAG *tStk;
  OPEN_INTERNAL_ENTITY *openEntityList;

  if (parser == NULL)
    return XML_FALSE;

  if (parser->m_parentParser)
    return XML_FALSE;
  /* move m_tagStack to m_freeTagList */
  tStk = parser->m_tagStack;
  while (tStk) {
    TAG *tag = tStk;
    tStk = tStk->parent;
    tag->parent = parser->m_freeTagList;
    moveToFreeBindingList(parser, tag->bindings);
    tag->bindings = NULL;
    parser->m_freeTagList = tag;
  }
  /* move m_openInternalEntities to m_freeInternalEntities */
  openEntityList = parser->m_openInternalEntities;
  while (openEntityList) {
    OPEN_INTERNAL_ENTITY *openEntity = openEntityList;
    openEntityList = openEntity->next;
    openEntity->next = parser->m_freeInternalEntities;
    parser->m_freeInternalEntities = openEntity;
  }
  moveToFreeBindingList(parser, parser->m_inheritedBindings);
  FREE(parser, parser->m_unknownEncodingMem);
  if (parser->m_unknownEncodingRelease)
    parser->m_unknownEncodingRelease(parser->m_unknownEncodingData);
  poolClear(&parser->m_tempPool);
  poolClear(&parser->m_temp2Pool);
  FREE(parser, (void *)parser->m_protocolEncodingName);
  parser->m_protocolEncodingName = NULL;
  parserInit(parser, encodingName);
  dtdReset(parser->m_dtd, &parser->m_mem);
  return XML_TRUE;
}