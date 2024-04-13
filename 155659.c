XML_SetUnparsedEntityDeclHandler(XML_Parser parser,
                                 XML_UnparsedEntityDeclHandler handler) {
  if (parser != NULL)
    parser->m_unparsedEntityDeclHandler = handler;
}