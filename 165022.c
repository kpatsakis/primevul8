void ApplyChannelMatrix::initDefaultMatrix()
{
	const double *matrix = NULL;
	if (m_inChannels==1 && m_outChannels==2)
	{
		static const double m[]={1,1};
		matrix = m;
	}
	else if (m_inChannels==1 && m_outChannels==4)
	{
		static const double m[]={1,1,0,0};
		matrix = m;
	}
	else if (m_inChannels==2 && m_outChannels==1)
	{
		static const double m[]={.5,.5};
		matrix = m;
	}
	else if (m_inChannels==2 && m_outChannels==4)
	{
		static const double m[]={1,0,0,1,0,0,0,0};
		matrix = m;
	}
	else if (m_inChannels==4 && m_outChannels==1)
	{
		static const double m[]={.5,.5,.5,.5};
		matrix = m;
	}
	else if (m_inChannels==4 && m_outChannels==2)
	{
		static const double m[]={1,0,1,0,0,1,0,1};
		matrix = m;
	}

	if (matrix)
	{
		std::copy(matrix, matrix + m_inChannels * m_outChannels, m_matrix);
	}
	else
	{
		for (int i=0; i < m_inChannels; i++)
			for (int j=0; j < m_outChannels; j++)
				m_matrix[j*m_inChannels + i] = (i==j) ? 1 : 0;
	}
}