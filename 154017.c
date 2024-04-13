bsearch_2short(short e1, short *ent1, short e2, short *ent2, int base,
	       short *indexarray, int nent)
{
    int n = nent;
    int k = 0;

    int e = e1 * base + e2;
    while (n > 0) {
	int nn = n / 2;
	int idx = indexarray[k + nn];
	int ne = ent1[idx] * base + ent2[idx];
	if (ne == e) {
	    k += nn;
	    break;
	}
	else if (ne < e) {
	    n -= nn + 1;
	    k += nn + 1;
	}
	else {
	    n = nn;
	}
    }
    return k;
}