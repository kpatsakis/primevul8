xmlNewEntityInputStream(xmlParserCtxtPtr ctxt, xmlEntityPtr entity) {
    xmlParserInputPtr input;

    if (entity == NULL) {
        xmlErrInternal(ctxt, "xmlNewEntityInputStream entity = NULL\n",
	               NULL);
	return(NULL);
    }
    if (xmlParserDebugEntities)
	xmlGenericError(xmlGenericErrorContext,
		"new input from entity: %s\n", entity->name);
    if (entity->content == NULL) {
	switch (entity->etype) {
            case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
	        xmlErrInternal(ctxt, "Cannot parse entity %s\n",
		               entity->name);
                break;
            case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
            case XML_EXTERNAL_PARAMETER_ENTITY:
		return(xmlLoadExternalEntity((char *) entity->URI,
		       (char *) entity->ExternalID, ctxt));
            case XML_INTERNAL_GENERAL_ENTITY:
	        xmlErrInternal(ctxt,
		      "Internal entity %s without content !\n",
		               entity->name);
                break;
            case XML_INTERNAL_PARAMETER_ENTITY:
	        xmlErrInternal(ctxt,
		      "Internal parameter entity %s without content !\n",
		               entity->name);
                break;
            case XML_INTERNAL_PREDEFINED_ENTITY:
	        xmlErrInternal(ctxt,
		      "Predefined entity %s without content !\n",
		               entity->name);
                break;
	}
	return(NULL);
    }
    input = xmlNewInputStream(ctxt);
    if (input == NULL) {
	return(NULL);
    }
    if (entity->URI != NULL)
	input->filename = (char *) xmlStrdup((xmlChar *) entity->URI);
    input->base = entity->content;
    if (entity->length == 0)
        entity->length = xmlStrlen(entity->content);
    input->cur = entity->content;
    input->length = entity->length;
    input->end = &entity->content[input->length];
    return(input);
}