XML_SetStartDoctypeDeclHandler(XML_Parser parser,
                               XML_StartDoctypeDeclHandler start) {
  if (parser != NULL)
    parser->m_startDoctypeDeclHandler = start;
}