errorProcessor(XML_Parser parser, const char *s, const char *end,
               const char **nextPtr) {
  UNUSED_P(s);
  UNUSED_P(end);
  UNUSED_P(nextPtr);
  return parser->m_errorCode;
}