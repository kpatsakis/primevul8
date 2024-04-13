entityTrackingReportStats(XML_Parser rootParser, ENTITY *entity,
                          const char *action, int sourceLine) {
  assert(! rootParser->m_parentParser);
  if (rootParser->m_entity_stats.debugLevel < 1)
    return;

#  if defined(XML_UNICODE)
  const char *const entityName = "[..]";
#  else
  const char *const entityName = entity->name;
#  endif

  fprintf(
      stderr,
      "expat: Entities(%p): Count %9d, depth %2d/%2d %*s%s%s; %s length %d (xmlparse.c:%d)\n",
      (void *)rootParser, rootParser->m_entity_stats.countEverOpened,
      rootParser->m_entity_stats.currentDepth,
      rootParser->m_entity_stats.maximumDepthSeen,
      (rootParser->m_entity_stats.currentDepth - 1) * 2, "",
      entity->is_param ? "%" : "&", entityName, action, entity->textLen,
      sourceLine);
}