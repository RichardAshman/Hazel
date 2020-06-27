#include "hzpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform\OpenGL\OpenGLBuffer.h"

// Note if made for more platforms then add something like the following
// #if HZ_PLATFORM_WINDOWS
// #include "Platform\Direct3D/Direct3DBuffer.h"
// #endif
// nb. would also have to suround the switch case in #if and #endif

namespace Hazel
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPI::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		
		}

		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t * indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::None: HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPI::OpenGL: return new OpenGLIndexBuffer(indices, size);

		}

		HZ_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}