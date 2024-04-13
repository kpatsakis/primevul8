textlist_feed()
{
    TextLine *p;
    if (_tl_lp2 != NULL) {
	p = _tl_lp2->ptr;
	_tl_lp2 = _tl_lp2->next;
	return p->line;
    }
    return NULL;
}