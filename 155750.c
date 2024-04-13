XML_SetHashSalt(XML_Parser parser, unsigned long hash_salt) {
  if (parser == NULL)
    return 0;
  if (parser->m_parentParser)
    return XML_SetHashSalt(parser->m_parentParser, hash_salt);
  /* block after XML_Parse()/XML_ParseBuffer() has been called */
  if (parser->m_parsingStatus.parsing == XML_PARSING
      || parser->m_parsingStatus.parsing == XML_SUSPENDED)
    return 0;
  parser->m_hash_secret_salt = hash_salt;
  return 1;
}