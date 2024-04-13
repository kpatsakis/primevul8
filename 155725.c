startParsing(XML_Parser parser) {
  /* hash functions must be initialized before setContext() is called */
  if (parser->m_hash_secret_salt == 0)
    parser->m_hash_secret_salt = generate_hash_secret_salt(parser);
  if (parser->m_ns) {
    /* implicit context only set for root parser, since child
       parsers (i.e. external entity parsers) will inherit it
    */
    return setContext(parser, implicitContext);
  }
  return XML_TRUE;
}