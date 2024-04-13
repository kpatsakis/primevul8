static int obj_size(struct pdf_struct *pdf, struct pdf_obj *obj)
{
    unsigned i = obj - pdf->objs;
    i++;
    if (i < pdf->nobjs) {
	int s = pdf->objs[i].start - obj->start - 4;
	if (s > 0)
	    return s;
    }
    return pdf->size - obj->start;
}