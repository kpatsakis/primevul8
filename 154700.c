long Tags::Tag::ParseSimpleTag(IMkvReader* pReader, long long pos,
                               long long size) {
  if (!ExpandSimpleTagsArray())
    return -1;

  SimpleTag& st = m_simple_tags[m_simple_tags_count++];
  st.Init();

  return st.Parse(pReader, pos, size);
}