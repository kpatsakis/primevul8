xmlValidGetPotentialChildren(xmlElementContent *ctree,
                             const xmlChar **names,
                             int *len, int max) {
    int i;

    if ((ctree == NULL) || (names == NULL) || (len == NULL))
        return(-1);
    if (*len >= max) return(*len);

    switch (ctree->type) {
	case XML_ELEMENT_CONTENT_PCDATA:
	    for (i = 0; i < *len;i++)
		if (xmlStrEqual(BAD_CAST "#PCDATA", names[i])) return(*len);
	    names[(*len)++] = BAD_CAST "#PCDATA";
	    break;
	case XML_ELEMENT_CONTENT_ELEMENT:
	    for (i = 0; i < *len;i++)
		if (xmlStrEqual(ctree->name, names[i])) return(*len);
	    names[(*len)++] = ctree->name;
	    break;
	case XML_ELEMENT_CONTENT_SEQ:
	    xmlValidGetPotentialChildren(ctree->c1, names, len, max);
	    xmlValidGetPotentialChildren(ctree->c2, names, len, max);
	    break;
	case XML_ELEMENT_CONTENT_OR:
	    xmlValidGetPotentialChildren(ctree->c1, names, len, max);
	    xmlValidGetPotentialChildren(ctree->c2, names, len, max);
	    break;
   }

   return(*len);
}