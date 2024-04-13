xmlValidateDtdFinal(xmlValidCtxtPtr ctxt, xmlDocPtr doc) {
    xmlDtdPtr dtd;
    xmlAttributeTablePtr table;
    xmlEntitiesTablePtr entities;

    if ((doc == NULL) || (ctxt == NULL)) return(0);
    if ((doc->intSubset == NULL) && (doc->extSubset == NULL))
	return(0);
    ctxt->doc = doc;
    ctxt->valid = 1;
    dtd = doc->intSubset;
    if ((dtd != NULL) && (dtd->attributes != NULL)) {
	table = (xmlAttributeTablePtr) dtd->attributes;
	xmlHashScan(table, (xmlHashScanner) xmlValidateAttributeCallback, ctxt);
    }
    if ((dtd != NULL) && (dtd->entities != NULL)) {
	entities = (xmlEntitiesTablePtr) dtd->entities;
	xmlHashScan(entities, (xmlHashScanner) xmlValidateNotationCallback,
		    ctxt);
    }
    dtd = doc->extSubset;
    if ((dtd != NULL) && (dtd->attributes != NULL)) {
	table = (xmlAttributeTablePtr) dtd->attributes;
	xmlHashScan(table, (xmlHashScanner) xmlValidateAttributeCallback, ctxt);
    }
    if ((dtd != NULL) && (dtd->entities != NULL)) {
	entities = (xmlEntitiesTablePtr) dtd->entities;
	xmlHashScan(entities, (xmlHashScanner) xmlValidateNotationCallback,
		    ctxt);
    }
    return(ctxt->valid);
}