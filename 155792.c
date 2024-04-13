XML_SetElementDeclHandler(XML_Parser parser, XML_ElementDeclHandler eldecl) {
  if (parser != NULL)
    parser->m_elementDeclHandler = eldecl;
}