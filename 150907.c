xmlValidateDtd(xmlValidCtxtPtr ctxt, xmlDocPtr doc, xmlDtdPtr dtd) {
    int ret;
    xmlDtdPtr oldExt, oldInt;
    xmlNodePtr root;

    if (dtd == NULL) return(0);
    if (doc == NULL) return(0);
    oldExt = doc->extSubset;
    oldInt = doc->intSubset;
    doc->extSubset = dtd;
    doc->intSubset = NULL;
    ret = xmlValidateRoot(ctxt, doc);
    if (ret == 0) {
	doc->extSubset = oldExt;
	doc->intSubset = oldInt;
	return(ret);
    }
    if (doc->ids != NULL) {
          xmlFreeIDTable(doc->ids);
          doc->ids = NULL;
    }
    if (doc->refs != NULL) {
          xmlFreeRefTable(doc->refs);
          doc->refs = NULL;
    }
    root = xmlDocGetRootElement(doc);
    ret = xmlValidateElement(ctxt, doc, root);
    ret &= xmlValidateDocumentFinal(ctxt, doc);
    doc->extSubset = oldExt;
    doc->intSubset = oldInt;
    return(ret);
}