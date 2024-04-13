xmlValidateCheckMixed(xmlValidCtxtPtr ctxt,
	              xmlElementContentPtr cont, const xmlChar *qname) {
    const xmlChar *name;
    int plen;
    name = xmlSplitQName3(qname, &plen);

    if (name == NULL) {
	while (cont != NULL) {
	    if (cont->type == XML_ELEMENT_CONTENT_ELEMENT) {
		if ((cont->prefix == NULL) && (xmlStrEqual(cont->name, qname)))
		    return(1);
	    } else if ((cont->type == XML_ELEMENT_CONTENT_OR) &&
	       (cont->c1 != NULL) &&
	       (cont->c1->type == XML_ELEMENT_CONTENT_ELEMENT)){
		if ((cont->c1->prefix == NULL) &&
		    (xmlStrEqual(cont->c1->name, qname)))
		    return(1);
	    } else if ((cont->type != XML_ELEMENT_CONTENT_OR) ||
		(cont->c1 == NULL) ||
		(cont->c1->type != XML_ELEMENT_CONTENT_PCDATA)){
		xmlErrValid(NULL, XML_DTD_MIXED_CORRUPT,
			"Internal: MIXED struct corrupted\n",
			NULL);
		break;
	    }
	    cont = cont->c2;
	}
    } else {
	while (cont != NULL) {
	    if (cont->type == XML_ELEMENT_CONTENT_ELEMENT) {
		if ((cont->prefix != NULL) &&
		    (xmlStrncmp(cont->prefix, qname, plen) == 0) &&
		    (xmlStrEqual(cont->name, name)))
		    return(1);
	    } else if ((cont->type == XML_ELEMENT_CONTENT_OR) &&
	       (cont->c1 != NULL) &&
	       (cont->c1->type == XML_ELEMENT_CONTENT_ELEMENT)){
		if ((cont->c1->prefix != NULL) &&
		    (xmlStrncmp(cont->c1->prefix, qname, plen) == 0) &&
		    (xmlStrEqual(cont->c1->name, name)))
		    return(1);
	    } else if ((cont->type != XML_ELEMENT_CONTENT_OR) ||
		(cont->c1 == NULL) ||
		(cont->c1->type != XML_ELEMENT_CONTENT_PCDATA)){
		xmlErrValid(ctxt, XML_DTD_MIXED_CORRUPT,
			"Internal: MIXED struct corrupted\n",
			NULL);
		break;
	    }
	    cont = cont->c2;
	}
    }
    return(0);
}