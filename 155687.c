prologInitProcessor(XML_Parser parser, const char *s, const char *end,
                    const char **nextPtr) {
  enum XML_Error result = initializeEncoding(parser);
  if (result != XML_ERROR_NONE)
    return result;
  parser->m_processor = prologProcessor;
  return prologProcessor(parser, s, end, nextPtr);
}