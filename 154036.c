bsearch_double(double e, double *ent, short *indexarray, int nent)
{
    int n = nent;
    int k = 0;

    while (n > 0) {
	int nn = n / 2;
	int idx = indexarray[k + nn];
	double ne = ent[idx];
	if (ne == e) {
	    k += nn;
	    break;
	}
	else if (ne > e) {
	    n -= nn + 1;
	    k += nn + 1;
	}
	else {
	    n = nn;
	}
    }
    return k;
}