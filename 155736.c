XML_SetStartElementHandler(XML_Parser parser, XML_StartElementHandler start) {
  if (parser != NULL)
    parser->m_startElementHandler = start;
}