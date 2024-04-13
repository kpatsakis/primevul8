cmsBool _MultiplyMatrix(cmsPipeline* Lut)
{
       cmsStage** pt1;
       cmsStage** pt2;
       cmsStage*  chain;
       cmsBool AnyOpt = FALSE;

       pt1 = &Lut->Elements;
       if (*pt1 == NULL) return AnyOpt;

       while (*pt1 != NULL) {

              pt2 = &((*pt1)->Next);
              if (*pt2 == NULL) return AnyOpt;

              if ((*pt1)->Implements == cmsSigMatrixElemType && (*pt2)->Implements == cmsSigMatrixElemType) {

                     // Get both matrices
                     _cmsStageMatrixData* m1 = (_cmsStageMatrixData*) cmsStageData(*pt1);
                     _cmsStageMatrixData* m2 = (_cmsStageMatrixData*) cmsStageData(*pt2);
                     cmsMAT3 res;
                     
                     // Input offset and output offset should be zero to use this optimization
                     if (m1->Offset != NULL || m2 ->Offset != NULL || 
                            cmsStageInputChannels(*pt1) != 3 || cmsStageOutputChannels(*pt1) != 3 ||                            
                            cmsStageInputChannels(*pt2) != 3 || cmsStageOutputChannels(*pt2) != 3)
                            return FALSE;

                     // Multiply both matrices to get the result
                     _cmsMAT3per(&res, (cmsMAT3*)m2->Double, (cmsMAT3*)m1->Double);

                     // Get the next in chain after the matrices
                     chain = (*pt2)->Next;

                     // Remove both matrices
                     _RemoveElement(pt2);
                     _RemoveElement(pt1);

                     // Now what if the result is a plain identity?                     
                     if (!isFloatMatrixIdentity(&res)) {

                            // We can not get rid of full matrix                            
                            cmsStage* Multmat = cmsStageAllocMatrix(Lut->ContextID, 3, 3, (const cmsFloat64Number*) &res, NULL);
                            if (Multmat == NULL) return FALSE;  // Should never happen

                            // Recover the chain
                            Multmat->Next = chain;
                            *pt1 = Multmat;
                     }

                     AnyOpt = TRUE;
              }
              else
                     pt1 = &((*pt1)->Next);
       }

       return AnyOpt;
}