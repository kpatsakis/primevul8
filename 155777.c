externalEntityInitProcessor(XML_Parser parser, const char *start,
                            const char *end, const char **endPtr) {
  enum XML_Error result = initializeEncoding(parser);
  if (result != XML_ERROR_NONE)
    return result;
  parser->m_processor = externalEntityInitProcessor2;
  return externalEntityInitProcessor2(parser, start, end, endPtr);
}