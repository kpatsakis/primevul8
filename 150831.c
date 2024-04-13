xmlValidateAttributeValue2(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
      const xmlChar *name, xmlAttributeType type, const xmlChar *value) {
    int ret = 1;
    switch (type) {
	case XML_ATTRIBUTE_IDREFS:
	case XML_ATTRIBUTE_IDREF:
	case XML_ATTRIBUTE_ID:
	case XML_ATTRIBUTE_NMTOKENS:
	case XML_ATTRIBUTE_ENUMERATION:
	case XML_ATTRIBUTE_NMTOKEN:
        case XML_ATTRIBUTE_CDATA:
	    break;
	case XML_ATTRIBUTE_ENTITY: {
	    xmlEntityPtr ent;

	    ent = xmlGetDocEntity(doc, value);
	    /* yeah it's a bit messy... */
	    if ((ent == NULL) && (doc->standalone == 1)) {
		doc->standalone = 0;
		ent = xmlGetDocEntity(doc, value);
	    }
	    if (ent == NULL) {
		xmlErrValidNode(ctxt, (xmlNodePtr) doc,
				XML_DTD_UNKNOWN_ENTITY,
   "ENTITY attribute %s reference an unknown entity \"%s\"\n",
		       name, value, NULL);
		ret = 0;
	    } else if (ent->etype != XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
		xmlErrValidNode(ctxt, (xmlNodePtr) doc,
				XML_DTD_ENTITY_TYPE,
   "ENTITY attribute %s reference an entity \"%s\" of wrong type\n",
		       name, value, NULL);
		ret = 0;
	    }
	    break;
        }
	case XML_ATTRIBUTE_ENTITIES: {
	    xmlChar *dup, *nam = NULL, *cur, save;
	    xmlEntityPtr ent;

	    dup = xmlStrdup(value);
	    if (dup == NULL)
		return(0);
	    cur = dup;
	    while (*cur != 0) {
		nam = cur;
		while ((*cur != 0) && (!IS_BLANK_CH(*cur))) cur++;
		save = *cur;
		*cur = 0;
		ent = xmlGetDocEntity(doc, nam);
		if (ent == NULL) {
		    xmlErrValidNode(ctxt, (xmlNodePtr) doc,
				    XML_DTD_UNKNOWN_ENTITY,
       "ENTITIES attribute %s reference an unknown entity \"%s\"\n",
			   name, nam, NULL);
		    ret = 0;
		} else if (ent->etype != XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
		    xmlErrValidNode(ctxt, (xmlNodePtr) doc,
				    XML_DTD_ENTITY_TYPE,
       "ENTITIES attribute %s reference an entity \"%s\" of wrong type\n",
			   name, nam, NULL);
		    ret = 0;
		}
		if (save == 0)
		    break;
		*cur = save;
		while (IS_BLANK_CH(*cur)) cur++;
	    }
	    xmlFree(dup);
	    break;
	}
	case XML_ATTRIBUTE_NOTATION: {
	    xmlNotationPtr nota;

	    nota = xmlGetDtdNotationDesc(doc->intSubset, value);
	    if ((nota == NULL) && (doc->extSubset != NULL))
		nota = xmlGetDtdNotationDesc(doc->extSubset, value);

	    if (nota == NULL) {
		xmlErrValidNode(ctxt, (xmlNodePtr) doc,
		                XML_DTD_UNKNOWN_NOTATION,
       "NOTATION attribute %s reference an unknown notation \"%s\"\n",
		       name, value, NULL);
		ret = 0;
	    }
	    break;
        }
    }
    return(ret);
}