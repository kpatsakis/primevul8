XML_GetAttributeInfo(XML_Parser parser) {
  if (parser == NULL)
    return NULL;
  return parser->m_attInfo;
}