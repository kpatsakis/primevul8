void Type_UcrBg_Free(struct _cms_typehandler_struct* self, void *Ptr)
{
   cmsUcrBg* Src = (cmsUcrBg*) Ptr;

   if (Src ->Ucr) cmsFreeToneCurve(Src ->Ucr);
   if (Src ->Bg)  cmsFreeToneCurve(Src ->Bg);
   if (Src ->Desc) cmsMLUfree(Src ->Desc);

   _cmsFree(self ->ContextID, Ptr);
}