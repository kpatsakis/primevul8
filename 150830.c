xmlScanIDAttributeDecl(xmlValidCtxtPtr ctxt, xmlElementPtr elem, int err) {
    xmlAttributePtr cur;
    int ret = 0;

    if (elem == NULL) return(0);
    cur = elem->attributes;
    while (cur != NULL) {
        if (cur->atype == XML_ATTRIBUTE_ID) {
	    ret ++;
	    if ((ret > 1) && (err))
		xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_MULTIPLE_ID,
	       "Element %s has too many ID attributes defined : %s\n",
		       elem->name, cur->name, NULL);
	}
	cur = cur->nexth;
    }
    return(ret);
}