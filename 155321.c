static const entity_ht *unescape_inverse_map(int all, int flags)
{
	int document_type = flags & ENT_HTML_DOC_TYPE_MASK;

	if (all) {
		switch (document_type) {
		case ENT_HTML_DOC_HTML401:
		case ENT_HTML_DOC_XHTML: /* but watch out for &apos;...*/
			return &ent_ht_html4;
		case ENT_HTML_DOC_HTML5:
			return &ent_ht_html5;
		default:
			return &ent_ht_be_apos;
		}
	} else {
		switch (document_type) {
		case ENT_HTML_DOC_HTML401:
			return &ent_ht_be_noapos;
		default:
			return &ent_ht_be_apos;
		}
	}
}