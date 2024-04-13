XML_SetDefaultHandler(XML_Parser parser, XML_DefaultHandler handler) {
  if (parser == NULL)
    return;
  parser->m_defaultHandler = handler;
  parser->m_defaultExpandInternalEntities = XML_FALSE;
}