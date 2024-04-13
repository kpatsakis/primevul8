xmlValidateCheckRefCallback(xmlListPtr ref_list, xmlValidCtxtPtr ctxt,
	                   const xmlChar *name) {
    xmlValidateMemo memo;

    if (ref_list == NULL)
	return;
    memo.ctxt = ctxt;
    memo.name = name;

    xmlListWalk(ref_list, xmlWalkValidateList, &memo);

}