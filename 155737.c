XML_SetNamespaceDeclHandler(XML_Parser parser,
                            XML_StartNamespaceDeclHandler start,
                            XML_EndNamespaceDeclHandler end) {
  if (parser == NULL)
    return;
  parser->m_startNamespaceDeclHandler = start;
  parser->m_endNamespaceDeclHandler = end;
}