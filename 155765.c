entityTrackingOnOpen(XML_Parser originParser, ENTITY *entity, int sourceLine) {
  const XML_Parser rootParser = getRootParserOf(originParser, NULL);
  assert(! rootParser->m_parentParser);

  rootParser->m_entity_stats.countEverOpened++;
  rootParser->m_entity_stats.currentDepth++;
  if (rootParser->m_entity_stats.currentDepth
      > rootParser->m_entity_stats.maximumDepthSeen) {
    rootParser->m_entity_stats.maximumDepthSeen++;
  }

  entityTrackingReportStats(rootParser, entity, "OPEN ", sourceLine);
}