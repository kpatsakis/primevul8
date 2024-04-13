XML_SetCdataSectionHandler(XML_Parser parser,
                           XML_StartCdataSectionHandler start,
                           XML_EndCdataSectionHandler end) {
  if (parser == NULL)
    return;
  parser->m_startCdataSectionHandler = start;
  parser->m_endCdataSectionHandler = end;
}