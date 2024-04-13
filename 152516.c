TiledInputFile::dataWindowForLevel (int lx, int ly) const
{
    try
    {
	return OPENEXR_IMF_INTERNAL_NAMESPACE::dataWindowForLevel (
	        _data->tileDesc,
	        _data->minX, _data->maxX,
	        _data->minY, _data->maxY,
	        lx, ly);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
	REPLACE_EXC (e, "Error calling dataWindowForLevel() on image "
                 "file \"" << fileName() << "\". " << e.what());
	throw;
    }
}