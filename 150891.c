xmlFreeID(xmlIDPtr id) {
    xmlDictPtr dict = NULL;

    if (id == NULL) return;

    if (id->doc != NULL)
        dict = id->doc->dict;

    if (id->value != NULL)
	DICT_FREE(id->value)
    if (id->name != NULL)
	DICT_FREE(id->name)
    xmlFree(id);
}