xmlCopyAttributeTable(xmlAttributeTablePtr table) {
    return((xmlAttributeTablePtr) xmlHashCopy(table,
				    (xmlHashCopier) xmlCopyAttribute));
}