TiledInputFile::levelWidth (int lx) const
{
    try
    {
        return levelSize (_data->minX, _data->maxX, lx,
			  _data->tileDesc.roundingMode);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
	REPLACE_EXC (e, "Error calling levelWidth() on image "
                 "file \"" << fileName() << "\". " << e.what());
	throw;
    }
}