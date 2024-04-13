XML_GetIdAttributeIndex(XML_Parser parser) {
  if (parser == NULL)
    return -1;
  return parser->m_idAttIndex;
}