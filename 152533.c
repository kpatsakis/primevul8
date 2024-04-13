TiledInputFile::numLevels () const
{
    if (levelMode() == RIPMAP_LEVELS)
	THROW (IEX_NAMESPACE::LogicExc, "Error calling numLevels() on image "
			      "file \"" << fileName() << "\" "
			      "(numLevels() is not defined for files "
			      "with RIPMAP level mode).");

    return _data->numXLevels;
}