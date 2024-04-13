XML_SetEndNamespaceDeclHandler(XML_Parser parser,
                               XML_EndNamespaceDeclHandler end) {
  if (parser != NULL)
    parser->m_endNamespaceDeclHandler = end;
}