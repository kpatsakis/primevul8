TInSliceInfo::TInSliceInfo (PixelType tifb,
                            PixelType tifl,
                            char *b,
                            size_t xs, size_t ys,
                            bool f, bool s,
                            double fv,
                            int xtc,
                            int ytc)
:
    typeInFrameBuffer (tifb),
    typeInFile (tifl),
    base (b),
    xStride (xs),
    yStride (ys),
    fill (f),
    skip (s),
    fillValue (fv),
    xTileCoords (xtc),
    yTileCoords (ytc)
{
    // empty
}