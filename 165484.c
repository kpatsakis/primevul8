MagickPrivate ResizeWeightingFunctionType GetResizeFilterWeightingType(
  const ResizeFilter *resize_filter)
{
  assert(resize_filter != (ResizeFilter *) NULL);
  assert(resize_filter->signature == MagickCoreSignature);
  return(resize_filter->filterWeightingType);
}