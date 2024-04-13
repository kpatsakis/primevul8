XML_SetAttlistDeclHandler(XML_Parser parser, XML_AttlistDeclHandler attdecl) {
  if (parser != NULL)
    parser->m_attlistDeclHandler = attdecl;
}