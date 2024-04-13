entityTrackingOnClose(XML_Parser originParser, ENTITY *entity, int sourceLine) {
  const XML_Parser rootParser = getRootParserOf(originParser, NULL);
  assert(! rootParser->m_parentParser);

  entityTrackingReportStats(rootParser, entity, "CLOSE", sourceLine);
  rootParser->m_entity_stats.currentDepth--;
}