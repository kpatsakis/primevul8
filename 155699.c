XML_SetNotationDeclHandler(XML_Parser parser, XML_NotationDeclHandler handler) {
  if (parser != NULL)
    parser->m_notationDeclHandler = handler;
}