TiledInputFile::isValidLevel (int lx, int ly) const
{
    if (lx < 0 || ly < 0)
	return false;

    if (levelMode() == MIPMAP_LEVELS && lx != ly)
	return false;

    if (lx >= numXLevels() || ly >= numYLevels())
	return false;

    return true;
}