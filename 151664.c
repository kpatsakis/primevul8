void* Type_vcgt_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    cmsToneCurve** OldCurves =  (cmsToneCurve**) Ptr;
    cmsToneCurve** NewCurves;

    NewCurves = ( cmsToneCurve**) _cmsCalloc(self ->ContextID, 3, sizeof(cmsToneCurve*));
    if (NewCurves == NULL) return NULL;

    NewCurves[0] = cmsDupToneCurve(OldCurves[0]);
    NewCurves[1] = cmsDupToneCurve(OldCurves[1]);
    NewCurves[2] = cmsDupToneCurve(OldCurves[2]);

    return (void*) NewCurves;

    cmsUNUSED_PARAMETER(n);
}