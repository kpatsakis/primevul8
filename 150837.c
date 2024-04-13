xmlCopyElementTable(xmlElementTablePtr table) {
    return((xmlElementTablePtr) xmlHashCopy(table,
		                            (xmlHashCopier) xmlCopyElement));
}