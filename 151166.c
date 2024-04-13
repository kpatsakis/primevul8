ccbaReadStream(FILE  *fp)
{
char      strbuf[256];
l_uint8   bval;
l_uint8  *datain, *dataout;
l_int32   i, j, startx, starty;
l_int32   offset, nib1, nib2;
l_int32   ncc, nb;
l_uint32  width, height, w, h, xoff, yoff;
size_t    inbytes, outbytes;
BOX      *box;
CCBORD   *ccb;
CCBORDA  *ccba;
NUMA     *na;
NUMAA    *step;

    PROCNAME("ccbaReadStream");

#if  !HAVE_LIBZ  /* defined in environ.h */
    return (CCBORDA *)ERROR_PTR("no libz: can't read data", procName, NULL);
#else

    if (!fp)
        return (CCBORDA *)ERROR_PTR("stream not open", procName, NULL);

    if ((datain = l_binaryReadStream(fp, &inbytes)) == NULL)
        return (CCBORDA *)ERROR_PTR("data not read from file", procName, NULL);
    dataout = zlibUncompress(datain, inbytes, &outbytes);
    LEPT_FREE(datain);
    if (!dataout)
        return (CCBORDA *)ERROR_PTR("dataout not made", procName, NULL);

    offset = 18;
    memcpy(strbuf, dataout, offset);
    strbuf[17] = '\0';
    if (memcmp(strbuf, "ccba:", 5) != 0) {
        LEPT_FREE(dataout);
        return (CCBORDA *)ERROR_PTR("file not type ccba", procName, NULL);
    }
    sscanf(strbuf, "ccba: %7d cc\n", &ncc);
/*    lept_stderr("ncc = %d\n", ncc); */
    if ((ccba = ccbaCreate(NULL, ncc)) == NULL) {
        LEPT_FREE(dataout);
        return (CCBORDA *)ERROR_PTR("ccba not made", procName, NULL);
    }

    memcpy(&width, dataout + offset, 4);
    offset += 4;
    memcpy(&height, dataout + offset, 4);
    offset += 4;
    ccba->w = width;
    ccba->h = height;
/*    lept_stderr("width = %d, height = %d\n", width, height); */

    for (i = 0; i < ncc; i++) {  /* should be ncc */
        ccb = ccbCreate(NULL);
        ccbaAddCcb(ccba, ccb);

        memcpy(&xoff, dataout + offset, 4);
        offset += 4;
        memcpy(&yoff, dataout + offset, 4);
        offset += 4;
        memcpy(&w, dataout + offset, 4);
        offset += 4;
        memcpy(&h, dataout + offset, 4);
        offset += 4;
        box = boxCreate(xoff, yoff, w, h);
        boxaAddBox(ccb->boxa, box, L_INSERT);
/*        lept_stderr("xoff = %d, yoff = %d, w = %d, h = %d\n",
                xoff, yoff, w, h); */

        memcpy(&nb, dataout + offset, 4);
        offset += 4;
/*        lept_stderr("num borders = %d\n", nb); */
        step = numaaCreate(nb);
        ccb->step = step;

        for (j = 0; j < nb; j++) {  /* should be nb */
            memcpy(&startx, dataout + offset, 4);
            offset += 4;
            memcpy(&starty, dataout + offset, 4);
            offset += 4;
            ptaAddPt(ccb->start, startx, starty);
/*            lept_stderr("startx = %d, starty = %d\n", startx, starty); */
            na = numaCreate(0);
            numaaAddNuma(step, na, L_INSERT);

            while(1) {
                bval = *(dataout + offset);
                offset++;
                nib1 = (bval >> 4);
                nib2 = bval & 0xf;
                if (nib1 != 8)
                    numaAddNumber(na, nib1);
                else
                    break;
                if (nib2 != 8)
                    numaAddNumber(na, nib2);
                else
                    break;
            }
        }
    }
    LEPT_FREE(dataout);
    return ccba;

#endif  /* !HAVE_LIBZ */
}