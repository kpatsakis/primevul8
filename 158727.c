static struct pdf_obj *find_obj(struct pdf_struct *pdf,
				struct pdf_obj *obj, uint32_t objid)
{
    unsigned j;
    unsigned i;

    /* search starting at previous obj (if exists) */
    if (obj != pdf->objs)
	i = obj - pdf->objs;
    else
	i = 0;
    for (j=i;j<pdf->nobjs;j++) {
	obj = &pdf->objs[j];
	if (obj->id == objid)
	    return obj;
    }
    /* restart search from beginning if not found */
    for (j=0;j<i;j++) {
	obj = &pdf->objs[j];
	if (obj->id == objid)
	    return obj;
    }
    return NULL;
}