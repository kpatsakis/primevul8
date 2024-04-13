long Tags::ParseTag(long long pos, long long size) {
  if (!ExpandTagsArray())
    return -1;

  Tag& t = m_tags[m_tags_count++];
  t.Init();

  return t.Parse(m_pSegment->m_pReader, pos, size);
}