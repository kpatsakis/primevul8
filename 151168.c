ccbaWriteStream(FILE     *fp,
                CCBORDA  *ccba)
{
char        strbuf[256];
l_uint8     bval;
l_uint8    *datain, *dataout;
l_int32     i, j, k, bx, by, bw, bh, val, startx, starty;
l_int32     ncc, nb, n;
l_uint32    w, h;
size_t      inbytes, outbytes;
L_BBUFFER  *bbuf;
CCBORD     *ccb;
NUMA       *na;
NUMAA      *naa;
PTA        *pta;

    PROCNAME("ccbaWriteStream");

#if  !HAVE_LIBZ  /* defined in environ.h */
    return ERROR_INT("no libz: can't write data", procName, 1);
#else

    if (!fp)
        return ERROR_INT("stream not open", procName, 1);
    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    if ((bbuf = bbufferCreate(NULL, 1000)) == NULL)
        return ERROR_INT("bbuf not made", procName, 1);

    ncc = ccbaGetCount(ccba);
    snprintf(strbuf, sizeof(strbuf), "ccba: %7d cc\n", ncc);
    bbufferRead(bbuf, (l_uint8 *)strbuf, 18);
    w = pixGetWidth(ccba->pix);
    h = pixGetHeight(ccba->pix);
    bbufferRead(bbuf, (l_uint8 *)&w, 4);  /* width */
    bbufferRead(bbuf, (l_uint8 *)&h, 4);  /* height */
    for (i = 0; i < ncc; i++) {
        ccb = ccbaGetCcb(ccba, i);
        if (boxaGetBoxGeometry(ccb->boxa, 0, &bx, &by, &bw, &bh)) {
            bbufferDestroy(&bbuf);
            ccbDestroy(&ccb);
            return ERROR_INT("bounding box not found", procName, 1);
        }
        bbufferRead(bbuf, (l_uint8 *)&bx, 4);  /* ulx of c.c. */
        bbufferRead(bbuf, (l_uint8 *)&by, 4);  /* uly of c.c. */
        bbufferRead(bbuf, (l_uint8 *)&bw, 4);  /* w of c.c. */
        bbufferRead(bbuf, (l_uint8 *)&bh, 4);  /* h of c.c. */
        if ((naa = ccb->step) == NULL) {
            ccbaGenerateStepChains(ccba);
            naa = ccb->step;
        }
        nb = numaaGetCount(naa);
        bbufferRead(bbuf, (l_uint8 *)&nb, 4);  /* number of borders in c.c. */
        pta = ccb->start;
        for (j = 0; j < nb; j++) {
            ptaGetIPt(pta, j, &startx, &starty);
            bbufferRead(bbuf, (l_uint8 *)&startx, 4); /* starting x in border */
            bbufferRead(bbuf, (l_uint8 *)&starty, 4); /* starting y in border */
            na = numaaGetNuma(naa, j, L_CLONE);
            n = numaGetCount(na);
            for (k = 0; k < n; k++) {
                numaGetIValue(na, k, &val);
                if (k % 2 == 0)
                    bval = (l_uint8)val << 4;
                else
                    bval |= (l_uint8)val;
                if (k % 2 == 1)
                    bbufferRead(bbuf, (l_uint8 *)&bval, 1); /* 2 border steps */
            }
            if (n % 2 == 1) {
                bval |= 0x8;
                bbufferRead(bbuf, (l_uint8 *)&bval, 1); /* end with 0xz8,   */
                                             /* where z = {0..7} */
            } else {  /* n % 2 == 0 */
                bval = 0x88;
                bbufferRead(bbuf, (l_uint8 *)&bval, 1);   /* end with 0x88 */
            }
            numaDestroy(&na);
        }
        ccbDestroy(&ccb);
    }

    datain = bbufferDestroyAndSaveData(&bbuf, &inbytes);
    dataout = zlibCompress(datain, inbytes, &outbytes);
    fwrite(dataout, 1, outbytes, fp);

    LEPT_FREE(datain);
    LEPT_FREE(dataout);
    return 0;

#endif  /* !HAVE_LIBZ */
}