XML_SetStartCdataSectionHandler(XML_Parser parser,
                                XML_StartCdataSectionHandler start) {
  if (parser != NULL)
    parser->m_startCdataSectionHandler = start;
}