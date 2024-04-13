xmlCopyNotationTable(xmlNotationTablePtr table) {
    return((xmlNotationTablePtr) xmlHashCopy(table,
				    (xmlHashCopier) xmlCopyNotation));
}