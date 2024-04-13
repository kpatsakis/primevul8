isoent_make_path_table_2(struct archive_write *a, struct vdd *vdd,
    int depth, int *dir_number)
{
	struct isoent *np;
	struct isoent **enttbl;
	struct path_table *pt;
	int i;

	pt = &vdd->pathtbl[depth];
	if (pt->cnt == 0) {
		pt->sorted = NULL;
		return (ARCHIVE_OK);
	}
	enttbl = malloc(pt->cnt * sizeof(struct isoent *));
	if (enttbl == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	pt->sorted = enttbl;
	for (np = pt->first; np != NULL; np = np->ptnext)
		*enttbl ++ = np;
	enttbl = pt->sorted;

	switch (vdd->vdd_type) {
	case VDD_PRIMARY:
	case VDD_ENHANCED:
#ifdef __COMPAR_FN_T
		qsort(enttbl, pt->cnt, sizeof(struct isoent *),
		    (__compar_fn_t)_compare_path_table);
#else
		qsort(enttbl, pt->cnt, sizeof(struct isoent *),
		    _compare_path_table);
#endif
		break;
	case VDD_JOLIET:
#ifdef __COMPAR_FN_T
		qsort(enttbl, pt->cnt, sizeof(struct isoent *),
		    (__compar_fn_t)_compare_path_table_joliet);
#else
		qsort(enttbl, pt->cnt, sizeof(struct isoent *),
		    _compare_path_table_joliet);
#endif
		break;
	}
	for (i = 0; i < pt->cnt; i++)
		enttbl[i]->dir_number = (*dir_number)++;

	return (ARCHIVE_OK);
}