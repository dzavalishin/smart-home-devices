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

#include "builder.h"
#include "settings.h"

#include <QThread>

Builder::Builder()
{
	verbose_log = false;

	process = new QProcess( this );
	connect( process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processNextTarget(int)) );
	connect( process, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()) );
	connect( process, SIGNAL(readyReadStandardError()), SLOT(readyReadStandardError()) );
}

Builder::~Builder()
{
	delete process;
}

Builder* Builder::instance()
{
	static Builder* theInstance = new Builder;
	return theInstance;
}

bool Builder::build( const QString& target, bool verbose )
{
	queue.append( target );
	if ( process->state() == QProcess::NotRunning )
	{
		verbose_log = verbose;
		if (verbose) {
			process->setProcessChannelMode( QProcess::MergedChannels );
		} else {
			process->setProcessChannelMode( QProcess::SeparateChannels );
		}
		QStringList env = QProcess::systemEnvironment();
		env.replaceInStrings(QRegExp("^PATH=(.*)", Qt::CaseInsensitive), "PATH=" + Settings::instance()->toolPath() + ";\\1" );
		env << "MAKE=make -j" + QString::number(QThread::idealThreadCount() + 1);
		process->setEnvironment( env );
		process->setWorkingDirectory( Settings::instance()->buildPath() );
		processNextTarget( 0 );
	}

	return true;
}

void Builder::processNextTarget( int exitCode )
{
	if ( queue.isEmpty() || exitCode != 0 )
	{
		queue.clear();
		emit done( exitCode );
		return;
	}

	runMake( queue.first() );
	queue.pop_front();
}

void Builder::runMake( const QString& target )
{
	emit message( tr("----- Running 'make %1' -----").arg( target ) );

	QStringList paramList;

	paramList << target;
	process->start( "make", paramList );
}

void Builder::readyReadStandardOutput()
{
	QByteArray ba = process->readAllStandardOutput();
	if ( verbose_log && !ba.isEmpty() )
		emit message( ba.trimmed() );
}

void Builder::readyReadStandardError()
{
	QByteArray ba = process->readAllStandardError();
	if ( !ba.isEmpty() )
		emit message( ba.trimmed() );
}

void Builder::stop()
{
	queue.clear();
	if ( process->state() != QProcess::NotRunning )
	{
		process->write("\x03"); // Send Ctrl+C
		process->waitForBytesWritten(-1);
		process->waitForFinished(-1);
		process->close();
	}
	emit message( tr("Build canceled by the user") );
}
