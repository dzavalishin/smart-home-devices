/*
 * Copyright (C) 2009 by Comm5 Tecnologia Ltda. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY COMM5 TECNOLOGIA LTDA AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

#if !defined( __DIRTRAVERSER_H__ )
#define __DIRTRAVERSER_H__

#include <QList>
#include <QRegExp>
#include <QFileInfo>

class AbstractFileCopyFilter
{
public:
	virtual ~AbstractFileCopyFilter() {}

	virtual bool onFile( const QFileInfo &fileInfo, const QString& dest ) = 0;
};

class AppDirCopyFilter : public AbstractFileCopyFilter
{
	bool onFile( const QFileInfo &fileInfo, const QString& dest );
};

class IccProjectCopyFilter : public AbstractFileCopyFilter
{
public:
	IccProjectCopyFilter( NutComponentModel* model );
	bool onFile( const QFileInfo& fileInfo, const QString& dest );
private:
	NutComponentModel* m_model;
};

class DirTraverser
{
public:
	DirTraverser();
	virtual ~DirTraverser();
	void insertFilter( AbstractFileCopyFilter* filter );
	void copyDir( const QString& src, const QString& dest );

private:
	QList<AbstractFileCopyFilter*> filters;
};

#endif // __DIRTRAVERSER_H__
