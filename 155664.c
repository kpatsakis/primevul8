XML_SetReturnNSTriplet(XML_Parser parser, int do_nst) {
  if (parser == NULL)
    return;
  /* block after XML_Parse()/XML_ParseBuffer() has been called */
  if (parser->m_parsingStatus.parsing == XML_PARSING
      || parser->m_parsingStatus.parsing == XML_SUSPENDED)
    return;
  parser->m_ns_triplets = do_nst ? XML_TRUE : XML_FALSE;
}