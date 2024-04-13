dv2sv(double *dv, short *iv, int size)
{
    int i, k, iw;
    short *indexarray;
    double *edv;
    double w = 0., x;

    indexarray = NewAtom_N(short, size);
    edv = NewAtom_N(double, size);
    for (i = 0; i < size; i++) {
	iv[i] = (short) ceil(dv[i]);
	edv[i] = (double)iv[i] - dv[i];
    }

    w = 0.;
    for (k = 0; k < size; k++) {
	x = edv[k];
	w += x;
	i = bsearch_double(x, edv, indexarray, k);
	if (k > i) {
	    int ii;
	    for (ii = k; ii > i; ii--)
		indexarray[ii] = indexarray[ii - 1];
	}
	indexarray[i] = k;
    }
    iw = min((int)(w + 0.5), size);
    if (iw <= 1)
	return;
    x = edv[(int)indexarray[iw - 1]];
    for (i = 0; i < size; i++) {
	k = indexarray[i];
	if (i >= iw && abs(edv[k] - x) > 1e-6)
	    break;
	iv[k]--;
    }
}