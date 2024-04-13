TiledInputFile::dataWindowForTile (int dx, int dy, int lx, int ly) const
{
    try
    {
	if (!isValidTile (dx, dy, lx, ly))
	    throw IEX_NAMESPACE::ArgExc ("Arguments not in valid range.");

        return OPENEXR_IMF_INTERNAL_NAMESPACE::dataWindowForTile (
                _data->tileDesc,
                _data->minX, _data->maxX,
                _data->minY, _data->maxY,
                dx, dy, lx, ly);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
	REPLACE_EXC (e, "Error calling dataWindowForTile() on image "
                 "file \"" << fileName() << "\". " << e.what());
	throw;
    }
}