void* Type_UcrBg_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    cmsUcrBg* Src = (cmsUcrBg*) Ptr;
    cmsUcrBg* NewUcrBg = (cmsUcrBg*) _cmsMallocZero(self ->ContextID, sizeof(cmsUcrBg));

    if (NewUcrBg == NULL) return NULL;

    NewUcrBg ->Bg   = cmsDupToneCurve(Src ->Bg);
    NewUcrBg ->Ucr  = cmsDupToneCurve(Src ->Ucr);
    NewUcrBg ->Desc = cmsMLUdup(Src ->Desc);

    return (void*) NewUcrBg;

    cmsUNUSED_PARAMETER(n);
}