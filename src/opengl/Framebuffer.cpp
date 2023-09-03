/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <opengl/Framebuffer.hpp>
#include <glad/gl.h>
#include <core/Window.hpp>
#include <cstddef>
#include <system_error>
#include <util/Log.hpp>

engine::Framebuffer::Framebuffer(unsigned int width, unsigned int height)
{
	// generate framebuffer
	glGenFramebuffers(1, &u_FrameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, u_FrameBufferID);

	// generate texture
	glGenTextures(1, &u_TextureID);
	glBindTexture(GL_TEXTURE_2D, u_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, u_TextureID, 0);

	glGenRenderbuffers(1, &u_RenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, u_RenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, u_RenderBufferID);

	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
		throw std::system_error(framebufferStatus, std::generic_category(), "Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

engine::Framebuffer::~Framebuffer()
{
	glDeleteTextures(1, &u_TextureID);
	glDeleteFramebuffers(1, &u_FrameBufferID);
	glDeleteRenderbuffers(1, &u_RenderBufferID);
}

void engine::Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, u_FrameBufferID);
}

void engine::Framebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int engine::Framebuffer::GetTextureID() const
{
	return u_TextureID;
}


/*
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/