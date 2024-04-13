XML_SetEntityDeclHandler(XML_Parser parser, XML_EntityDeclHandler handler) {
  if (parser != NULL)
    parser->m_entityDeclHandler = handler;
}