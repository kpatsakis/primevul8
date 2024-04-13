cmsBool  Read8bitTables(cmsContext ContextID, cmsIOHANDLER* io, cmsPipeline* lut, int nChannels)
{
    cmsUInt8Number* Temp = NULL;
    int i, j;
    cmsToneCurve* Tables[cmsMAXCHANNELS];

    if (nChannels > cmsMAXCHANNELS) return FALSE;
    if (nChannels <= 0) return FALSE;

    memset(Tables, 0, sizeof(Tables));

    Temp = (cmsUInt8Number*) _cmsMalloc(ContextID, 256);
    if (Temp == NULL) return FALSE;

    for (i=0; i < nChannels; i++) {
        Tables[i] = cmsBuildTabulatedToneCurve16(ContextID, 256, NULL);
        if (Tables[i] == NULL) goto Error;
    }

    for (i=0; i < nChannels; i++) {

        if (io ->Read(io, Temp, 256, 1) != 1) goto Error;

        for (j=0; j < 256; j++)
            Tables[i]->Table16[j] = (cmsUInt16Number) FROM_8_TO_16(Temp[j]);
    }

    _cmsFree(ContextID, Temp);
    Temp = NULL;

    if (!cmsPipelineInsertStage(lut, cmsAT_END, cmsStageAllocToneCurves(ContextID, nChannels, Tables)))
        goto Error;

    for (i=0; i < nChannels; i++)
        cmsFreeToneCurve(Tables[i]);

    return TRUE;

Error:
    for (i=0; i < nChannels; i++) {
        if (Tables[i]) cmsFreeToneCurve(Tables[i]);
    }

    if (Temp) _cmsFree(ContextID, Temp);
    return FALSE;
}