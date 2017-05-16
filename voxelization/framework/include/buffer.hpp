#pragma once

#include "gl.hpp"

namespace gpupro {

	// A buffer is a pure memory block on GPU side.
	class Buffer
	{
	public:
		// The type of a buffer determines its main purpose.
		// The buffer will be bound to that slot for several operations.
		// However, you can always bind the same buffer for different
		// purposes.
		enum class Type
		{
			VERTEX = GL_ARRAY_BUFFER,
			INDEX = GL_ELEMENT_ARRAY_BUFFER,
			SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER,
			TEXTURE = GL_TEXTURE_BUFFER,
			UNIFORM = GL_UNIFORM_BUFFER,
			ATOMIC = GL_ATOMIC_COUNTER_BUFFER,
			INDIRECT_DISPATCH = GL_DISPATCH_INDIRECT_BUFFER,
			INDIRECT_DRAW = GL_DRAW_INDIRECT_BUFFER,
			TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER,
		};

		enum Usage
		{
			// Allow glBufferSubData updates for this buffer.
			SUB_DATA_UPDATE = GL_DYNAMIC_STORAGE_BIT,
			// Allow read accesses from CPU side
			MAP_READ = GL_MAP_READ_BIT,
			// Allow write accesses form CPU side
			MAP_WRITE = GL_MAP_WRITE_BIT,
			// The buffer can always be mapped. Even during draw
			// CPU side accesses are possible.
			// Without this bit the buffer must be unmapped before
			// used by the GPU.
			// To make writes visible use flush() or receive().
			MAP_PERSISTENT = GL_MAP_PERSISTENT_BIT,
			// You don't need to call flush(). Consider this as
			// less performant. receive() is still necessary for
			// synchronization issues.
			MAP_COHERENT = GL_MAP_COHERENT_BIT
		};

		// Create a raw buffer for data. Buffers created with this method
		// cannot be used as texture buffers!
		// _data: data for initialization.
		Buffer(Type _type, GLuint _elementSize, GLuint _numElements, Usage _usageBits = Usage(), const GLvoid* _data = nullptr);
		~Buffer();
		// Move but not copy-able
		Buffer(Buffer&& _rhs);
		Buffer(const Buffer&) = delete;
		Buffer& operator = (Buffer&& _rhs);
		Buffer& operator = (const Buffer& _rhs) = delete;

		// Bind as vertex buffer
		// _offset: offset to the first element in number of elements
		void bindAsVertexBuffer(GLuint _bindingIndex, GLuint _offset = 0);
		// Bind to GL_ELEMENT_ARRAY_BUFFER (index buffer)
		void bindAsIndexBuffer();
		// Bind as uniform buffer
		// _bindingIndex: binding slot of the uniform buffer
		// _offset: it is possible to bind a part of a larger buffer.
		//		This gives the offset in byte to the begin of the range.
		// _size: size of the range in bytes. -1 binds the entire buffer.
		void bindAsUniformBuffer(GLuint _bindingIndex, GLintptr _offset = 0, GLsizeiptr _size = GLsizeiptr(-1));

		// Upload a small chunk of data to a specific position.
		// Requires Usage::SUB_DATA_UPDATE.
		void subDataUpdate(GLintptr _offset, GLsizei _size, const GLvoid* _data);


		// Set the entire buffer content to zero.
		void clear();


		enum MappingFlags
		{
			READ = GL_MAP_READ_BIT,
			WRITE = GL_MAP_WRITE_BIT,
			// Open a persistent mapping.
			// Coherent is added automatically if set in Usage during initialization.
			// Otherwise, GL_MAP_FLUSH_EXPLICIT_BIT will be set.
			PERSISTENT = GL_MAP_PERSISTENT_BIT,
			// Discard the previous buffer contents. Cannot be used with READ.
			INVALIDATE = GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT,
			// Do not wait for GPU operations on the buffer. Map it immediately.
			ASYNCHRONOUS = GL_MAP_UNSYNCHRONIZED_BIT,
		};

		// Mapping is the recommended way to handle buffer data.
		// You can read/write the returned pointer until unmap(). If mapped
		// persistently you must make changes visible through flush() and receive().
		void* map(MappingFlags _access, GLintptr _offset = 0, GLsizei _size = GLsizei(-1));
		// Release mapping (pointer becomes invalid), a flush is done automatically
		// except for PERSISTENT INCOHERENT buffers.
		void unmap();
		// Make CPU sided writes visible for the GPU.
		void flush();
		// Make GPU sided writes visible on CPU (slow sync!).
		// Mapping with the READ flag is doing this automatically except
		// ASYNCHRONOUS is set.
		void receive();

		GLuint numElements() const { return m_size / m_elementSize; }

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
		Type m_type;
		GLsizei m_size;
		GLuint m_elementSize;
		Usage m_usage;

		GLintptr m_mappedOffset;
		GLsizei m_mappedSize;
	};

} // namespace gpupro