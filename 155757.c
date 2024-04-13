XML_SetCharacterDataHandler(XML_Parser parser,
                            XML_CharacterDataHandler handler) {
  if (parser != NULL)
    parser->m_characterDataHandler = handler;
}