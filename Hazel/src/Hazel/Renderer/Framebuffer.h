#pragma once

#include "Hazel/Core/Base.h"

namespace Hazel {

	struct FramebufferSpecification
	{
		//TODO color, depth, 
		//FramebufferFormat Format = ;
		uint32_t Width, Height;
		uint32_t Sample = 1;

		bool SwapChainTarget = false; // going to be = to glBindFramebuffer(0) eg are we rendering to the screen or not
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		//virtual FramebufferSpecification& GetSpecification() = 0; // Could have this aswell but nothing would notice it being changed. Better to have a changeSpec function
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

	};
}