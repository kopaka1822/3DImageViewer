#pragma once

#include "gl.hpp"

namespace gpupro {

	// A query object can be used to get information from GPU like
	// the number of generated samples (occlusion query), or the time
	// which passed between the two commands begin() and end().
	//
	// There can be only one active query object of each type!
	// Thereby, an object is active between the begin() and end() call.
	//
	// Independent of the type this class will always return a double:
	//  - num samples (casted from int)
	//  - any samples is 0.0 or 1.0
	//  - num primitives (casted from int)
	//  - time in ms
	class Query
	{
	public:
		enum class Type {
			SAMPLES_PASSED = GL_SAMPLES_PASSED,
			ANY_SAMPLES_PASSED = GL_ANY_SAMPLES_PASSED,
			ANY_SAMPLES_PASSED_CONSERVATIVE = GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
			PRIMITIVES_GENERATED = GL_PRIMITIVES_GENERATED,
			TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
			TIME_ELAPSED = GL_TIME_ELAPSED
		};

		Query(Type _type);
		~Query();
		// Move but not copy-able
		Query(Query&& _rhs);
		Query(const Query&) = delete;
		Query& operator = (Query&& _rhs);
		Query& operator = (const Query&) = delete;

		// Begin a query.
		// A query object can only be used in one query at the same time.
		void begin();

		// End a query on GPU side (asynchronous).
		void end();

		// Get the result. Busy waiting until the GPU reached the query.
		// To prevent waiting use multiple objects and use available() to
		// check if a query is ready.
		void receive(bool _wait = true);

		// Check if the results are ready
		bool available() const;

		// Get the latest received timing. This is not
		// necessarily from the last end() call.
		double latest() const { return m_latestResult; }

		// A smoothed number over the passed frames.
		// This is not the true average over all frames.
		double avg() const { return m_avgResult; }

		// Get the extremal values of all uses.
		double min() const { return m_max; }
		double max() const { return m_min; }

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
		Type m_type;

		double m_latestResult;
		double m_avgResult;
		double m_min, m_max;
	};

}
