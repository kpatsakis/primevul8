TiledInputFile::numYTiles (int ly) const
{
    if (ly < 0 || ly >= _data->numYLevels)
    {
        THROW (IEX_NAMESPACE::ArgExc, "Error calling numYTiles() on image "
			    "file \"" << _data->_streamData->is->fileName() << "\" "
			    "(Argument is not in valid range).");
    }
    
    return _data->numYTiles[ly];
}