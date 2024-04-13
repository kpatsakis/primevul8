xmlValidateElementDecl(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                       xmlElementPtr elem) {
    int ret = 1;
    xmlElementPtr tst;

    CHECK_DTD;

    if (elem == NULL) return(1);

#if 0
#ifdef LIBXML_REGEXP_ENABLED
    /* Build the regexp associated to the content model */
    ret = xmlValidBuildContentModel(ctxt, elem);
#endif
#endif

    /* No Duplicate Types */
    if (elem->etype == XML_ELEMENT_TYPE_MIXED) {
	xmlElementContentPtr cur, next;
        const xmlChar *name;

	cur = elem->content;
	while (cur != NULL) {
	    if (cur->type != XML_ELEMENT_CONTENT_OR) break;
	    if (cur->c1 == NULL) break;
	    if (cur->c1->type == XML_ELEMENT_CONTENT_ELEMENT) {
		name = cur->c1->name;
		next = cur->c2;
		while (next != NULL) {
		    if (next->type == XML_ELEMENT_CONTENT_ELEMENT) {
		        if ((xmlStrEqual(next->name, name)) &&
			    (xmlStrEqual(next->prefix, cur->c1->prefix))) {
			    if (cur->c1->prefix == NULL) {
				xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_CONTENT_ERROR,
		   "Definition of %s has duplicate references of %s\n",
				       elem->name, name, NULL);
			    } else {
				xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_CONTENT_ERROR,
		   "Definition of %s has duplicate references of %s:%s\n",
				       elem->name, cur->c1->prefix, name);
			    }
			    ret = 0;
			}
			break;
		    }
		    if (next->c1 == NULL) break;
		    if (next->c1->type != XML_ELEMENT_CONTENT_ELEMENT) break;
		    if ((xmlStrEqual(next->c1->name, name)) &&
		        (xmlStrEqual(next->c1->prefix, cur->c1->prefix))) {
			if (cur->c1->prefix == NULL) {
			    xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_CONTENT_ERROR,
	       "Definition of %s has duplicate references to %s\n",
				   elem->name, name, NULL);
			} else {
			    xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_CONTENT_ERROR,
	       "Definition of %s has duplicate references to %s:%s\n",
				   elem->name, cur->c1->prefix, name);
			}
			ret = 0;
		    }
		    next = next->c2;
		}
	    }
	    cur = cur->c2;
	}
    }

    /* VC: Unique Element Type Declaration */
    tst = xmlGetDtdElementDesc(doc->intSubset, elem->name);
    if ((tst != NULL ) && (tst != elem) &&
	((tst->prefix == elem->prefix) ||
	 (xmlStrEqual(tst->prefix, elem->prefix))) &&
	(tst->etype != XML_ELEMENT_TYPE_UNDEFINED)) {
	xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_ELEM_REDEFINED,
	                "Redefinition of element %s\n",
		       elem->name, NULL, NULL);
	ret = 0;
    }
    tst = xmlGetDtdElementDesc(doc->extSubset, elem->name);
    if ((tst != NULL ) && (tst != elem) &&
	((tst->prefix == elem->prefix) ||
	 (xmlStrEqual(tst->prefix, elem->prefix))) &&
	(tst->etype != XML_ELEMENT_TYPE_UNDEFINED)) {
	xmlErrValidNode(ctxt, (xmlNodePtr) elem, XML_DTD_ELEM_REDEFINED,
	                "Redefinition of element %s\n",
		       elem->name, NULL, NULL);
	ret = 0;
    }
    /* One ID per Element Type
     * already done when registering the attribute
    if (xmlScanIDAttributeDecl(ctxt, elem) > 1) {
	ret = 0;
    } */
    return(ret);
}