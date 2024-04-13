check_cell_width(short *tabwidth, short *cellwidth,
		 short *col, short *colspan, short maxcell,
		 short *indexarray, int space, int dir)
{
    int i, j, k, bcol, ecol;
    int swidth, width;

    for (k = 0; k <= maxcell; k++) {
	j = indexarray[k];
	if (cellwidth[j] <= 0)
	    continue;
	bcol = col[j];
	ecol = bcol + colspan[j];
	swidth = 0;
	for (i = bcol; i < ecol; i++)
	    swidth += tabwidth[i];

	width = cellwidth[j] - (colspan[j] - 1) * space;
	if (width > swidth) {
	    int w = (width - swidth) / colspan[j];
	    int r = (width - swidth) % colspan[j];
	    for (i = bcol; i < ecol; i++)
		tabwidth[i] += w;
	    /* dir {0: horizontal, 1: vertical} */
	    if (dir == 1 && r > 0)
		r = colspan[j];
	    for (i = 1; i <= r; i++)
		tabwidth[ecol - i]++;
	}
    }
}