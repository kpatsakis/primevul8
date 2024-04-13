TiledInputFile::numXTiles (int lx) const
{
    if (lx < 0 || lx >= _data->numXLevels)
    {
        THROW (IEX_NAMESPACE::ArgExc, "Error calling numXTiles() on image "
			    "file \"" << _data->_streamData->is->fileName() << "\" "
			    "(Argument is not in valid range).");

    }
    
    return _data->numXTiles[lx];
}