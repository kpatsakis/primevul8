XML_SetEndElementHandler(XML_Parser parser, XML_EndElementHandler end) {
  if (parser != NULL)
    parser->m_endElementHandler = end;
}