cmsBool  isFloatMatrixIdentity(const cmsMAT3* a)
{
       cmsMAT3 Identity;
       int i, j;

       _cmsMAT3identity(&Identity);

       for (i = 0; i < 3; i++)
              for (j = 0; j < 3; j++)
                     if (!CloseEnoughFloat(a->v[i].n[j], Identity.v[i].n[j])) return FALSE;

       return TRUE;
}