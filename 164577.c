static int TraceQuadraticBezier(FT_Vector *control,FT_Vector *to,
  DrawInfo *draw_info)
{
  AffineMatrix
    affine;

  char
    path[MagickPathExtent];

  affine=draw_info->affine;
  (void) FormatLocaleString(path,MagickPathExtent,"Q%g,%g %g,%g",affine.tx+
    control->x/64.0,affine.ty-control->y/64.0,affine.tx+to->x/64.0,affine.ty-
    to->y/64.0);
  (void) ConcatenateString(&draw_info->primitive,path);
  return(0);
}