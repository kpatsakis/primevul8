getRootParserOf(XML_Parser parser, unsigned int *outLevelDiff) {
  XML_Parser rootParser = parser;
  unsigned int stepsTakenUpwards = 0;
  while (rootParser->m_parentParser) {
    rootParser = rootParser->m_parentParser;
    stepsTakenUpwards++;
  }
  assert(! rootParser->m_parentParser);
  if (outLevelDiff != NULL) {
    *outLevelDiff = stepsTakenUpwards;
  }
  return rootParser;
}