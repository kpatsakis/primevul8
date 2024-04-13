XML_SetElementHandler(XML_Parser parser, XML_StartElementHandler start,
                      XML_EndElementHandler end) {
  if (parser == NULL)
    return;
  parser->m_startElementHandler = start;
  parser->m_endElementHandler = end;
}