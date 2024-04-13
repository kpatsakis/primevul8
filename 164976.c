status ModuleState::initFileModule(AFfilehandle file, _Track *track)
{
	int compressionIndex = _af_compression_index_from_id(track->f.compressionType);
	const _CompressionUnit *unit = &_af_compression[compressionIndex];

	if (af_fseek(file->fh, track->fpos_first_frame, SEEK_SET) < 0)
	{
		_af_error(AF_BAD_LSEEK, "unable to position file handle at beginning of sound data");
		return AF_FAIL;
	}

	AFframecount chunkFrames;
	if (file->access == _AF_READ_ACCESS)
		m_fileModule = unit->initdecompress(track, file->fh, file->seekok,
			file->fileFormat == AF_FILE_RAWDATA, &chunkFrames);
	else
		m_fileModule = unit->initcompress(track, file->fh, file->seekok,
			file->fileFormat == AF_FILE_RAWDATA, &chunkFrames);

	track->filemodhappy = true;

	return AF_SUCCEED;
}