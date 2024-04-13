TiledInputFile::levelHeight (int ly) const
{
    try
    {
        return levelSize (_data->minY, _data->maxY, ly,
                          _data->tileDesc.roundingMode);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
	REPLACE_EXC (e, "Error calling levelHeight() on image "
                 "file \"" << fileName() << "\". " << e.what());
	throw;
    }
}