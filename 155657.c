XML_GetSpecifiedAttributeCount(XML_Parser parser) {
  if (parser == NULL)
    return -1;
  return parser->m_nSpecifiedAtts;
}