xmlCopyNotation(xmlNotationPtr nota) {
    xmlNotationPtr cur;

    cur = (xmlNotationPtr) xmlMalloc(sizeof(xmlNotation));
    if (cur == NULL) {
	xmlVErrMemory(NULL, "malloc failed");
	return(NULL);
    }
    if (nota->name != NULL)
	cur->name = xmlStrdup(nota->name);
    else
	cur->name = NULL;
    if (nota->PublicID != NULL)
	cur->PublicID = xmlStrdup(nota->PublicID);
    else
	cur->PublicID = NULL;
    if (nota->SystemID != NULL)
	cur->SystemID = xmlStrdup(nota->SystemID);
    else
	cur->SystemID = NULL;
    return(cur);
}