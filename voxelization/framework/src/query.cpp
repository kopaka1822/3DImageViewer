#include "query.hpp"

#include <algorithm>

gpupro::Query::Query(Type _type) :
	m_type(_type)
{
	glGenQueries(1, &m_id);
	m_latestResult = 0.0;
	m_avgResult = 0.0;
	m_min = 1.0;	// Marks an object without samples
	m_max = 0.0;
}

gpupro::Query::~Query()
{
	glDeleteQueries(1, &m_id);
}

gpupro::Query::Query(Query && _rhs) :
	m_id(_rhs.m_id),
	m_type(_rhs.m_type),
	m_latestResult(_rhs.m_latestResult),
	m_avgResult(_rhs.m_avgResult),
	m_min(_rhs.m_min),
	m_max(_rhs.m_max)
{
	_rhs.m_id = 0;
}

gpupro::Query& gpupro::Query::operator = (Query && _rhs)
{
	glDeleteQueries(1, &m_id);

	m_id = _rhs.m_id;
	m_type = _rhs.m_type;
	m_latestResult = _rhs.m_latestResult;
	m_avgResult = _rhs.m_avgResult;
	m_min = _rhs.m_min;
	m_max = _rhs.m_max;
	_rhs.m_id = 0;

	return *this;
}

void gpupro::Query::begin()
{
	glBeginQuery(static_cast<GLenum>(m_type), m_id);
}

void gpupro::Query::end()
{
	glEndQuery(static_cast<GLenum>(m_type));
}

void gpupro::Query::receive(bool _wait)
{
	GLuint res = 0xff00ff00;
	glGetQueryObjectuiv(m_id, _wait ? GL_QUERY_RESULT : GL_QUERY_RESULT_NO_WAIT, &res);
	// Only use the result if the query passed without problems and is ready by now.
	if(res != 0xff00ff00)
	{
		m_latestResult = static_cast<double>(res);
		// The results of the time query are given in ns. Convert to ms.
		if(m_type == Type::TIME_ELAPSED)
			m_latestResult /= 1000000.0;
		if(m_min > m_max)
		{
			m_avgResult = m_latestResult;
			m_min = m_max = m_latestResult;
		} else {
			m_avgResult += (m_latestResult - m_avgResult) * 0.02;
			m_min = std::min(m_min, m_latestResult);
			m_max = std::max(m_max, m_latestResult);
		}
	}
}

bool gpupro::Query::available() const
{
	GLuint res;
	glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, &res);
	return res != 0;
}
