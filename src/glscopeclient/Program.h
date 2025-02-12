/***********************************************************************************************************************
*                                                                                                                      *
* glscopeclient                                                                                                        *
*                                                                                                                      *
* Copyright (c) 2012-2020 Andrew D. Zonenberg                                                                          *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief  Declaration of Program
 */
#ifndef Program_h
#define Program_h

#include "Shader.h"
#include "Texture.h"

class Program
{
public:
	Program();
	virtual ~Program();

	void Destroy();

	bool Link();

	void Add(Shader& shader);

	operator GLuint()
	{ return m_handle; }

	GLint GetAttributeLocation(const char* name)
	{
		//Check the cache rather than going to the GL driver if we can avoid it
		auto pos = m_attribMap.find(name);
		if(pos != m_attribMap.end())
			return pos->second;

		//Nope, ask and add to cache
		GLint index = glGetAttribLocation(m_handle, name);
		m_attribMap[name] = index;
		return index;
	}

	GLint GetUniformLocation(const char* name)
	{
		//Check the cache rather than going to the GL driver if we can avoid it
		auto pos = m_uniformMap.find(name);
		if(pos != m_uniformMap.end())
			return pos->second;

		//Nope, ask and add to cache
		GLint index = glGetUniformLocation(m_handle, name);
		m_uniformMap[name] = index;
		if(index < 0)
			LogError("Uniform %s couldn't be found\n", name);
		return index;
	}


	void Bind()
	{ glUseProgram(m_handle); }

	//these functions work on the current VAO
	void EnableVertexArray(const char* name)
	{ glEnableVertexAttribArray(GetAttributeLocation(name)); }

	void SetVertexAttribPointer(const char* name, int size = 3, size_t offset = 0, size_t stride = 0)
	{ glVertexAttribPointer(GetAttributeLocation(name), size, GL_FLOAT, GL_FALSE, stride, (void*)offset); }

	void SetUniform(double f, const char* name)
	{ glUniform1f(GetUniformLocation(name), f); }

	void SetUniform(float f, const char* name)
	{ glUniform1f(GetUniformLocation(name), f); }

	void SetUniform(int i, const char* name)
	{ glUniform1i(GetUniformLocation(name), i); }

	void SetUniform(Texture& tex, const char* name, int texid = 0)
	{
		glActiveTexture(GL_TEXTURE0 + texid);
		tex.Bind();
		glUniform1i(GetUniformLocation(name), texid);
	}

#if false //TODO: Delete this, no longer needed?
	void SetImageUniform(Texture& tex, const char* name, int texid = 0)
	{
		glActiveTexture(GL_TEXTURE0 + texid);
		tex.Bind();
		glUniform1i(GetUniformLocation(name), texid);
		glBindImageTexture(texid, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}
#endif

	void DispatchCompute(GLuint x, GLuint y, GLuint z)
	{ glDispatchCompute(x,y,z); }

	void MemoryBarrier()
	{ glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); }

protected:
	GLuint m_handle;

	//Map of attribute names to locations
	std::map<std::string, GLint> m_attribMap;
	std::map<std::string, GLint> m_uniformMap;
};

#endif
