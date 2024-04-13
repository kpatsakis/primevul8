ApplyChannelMatrix::ApplyChannelMatrix(FormatCode format, bool isReading,
	int inChannels, int outChannels,
	double minClip, double maxClip, const double *matrix) :
	m_format(format),
	m_inChannels(inChannels),
	m_outChannels(outChannels),
	m_minClip(minClip),
	m_maxClip(maxClip),
	m_matrix(NULL)
{
	m_matrix = new double[m_inChannels * m_outChannels];
	if (matrix)
	{
		if (isReading)
		{
			// Copy channel matrix for reading.
			std::copy(matrix, matrix + m_inChannels * m_outChannels, m_matrix);
		}
		else
		{
			// Transpose channel matrix for writing.
			for (int i=0; i < inChannels; i++)
				for (int j=0; j < outChannels; j++)
					m_matrix[j*inChannels + i] = matrix[i*outChannels + j];
		}
	}
	else
	{
		initDefaultMatrix();
	}
}