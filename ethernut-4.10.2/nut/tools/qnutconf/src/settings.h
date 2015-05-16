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

#if !defined( __SETTINGS_H__ )
#define __SETTINGS_H__

#include <QDir>
#include <QObject>

class Settings : public QObject
{
	Q_OBJECT

public:
	~Settings();
	
	bool load( const QString& fileName = QString() );
	void save();

	static Settings* instance();
	static QString findRelativePath( const QString& filename );


	QString repository() const { return repositoryFile; }
	void setRepository(const QString& val)	{ repositoryFile = val;	}
	QString configFileName() const { return m_configFileName; }
	void setConfigFileName(const QString& val) { m_configFileName = val; }
	QString buildPath() const { return m_buildPath; }
	void setBuildPath(const QString& val) { m_buildPath = val; }
	QStringList includePath() const { return m_includePath; }
	void setIncludePath( const QStringList& val) { m_includePath = val; }
	QString installPath() const { return m_installPath; }
	void setInstallPath(const QString& val) { m_installPath = val; }
	QString sourceDir() const { return m_sourceDir; }
	void setSourceDir(const QString& val) { m_sourceDir = val; }
	QString appDir() const { return m_appDir; }
	void setAppDir(const QString& val) { m_appDir = val; }
	QString toolPath() const { return m_toolPath; }
	void setToolPath(const QString& val) { m_toolPath = val; }
	QString targetPlatform() const { return m_targetPlatform; }
	void setTargetPlatform(const QString& val) { m_targetPlatform = val; }
	QString programmer() const { return m_programmer; }
	void setProgrammer(const QString& val) { m_programmer = val; }
	bool absolutePathInSamples() { return m_absolutePathInSamples; }
	void setAbsolutePathInSamples( bool val ) { m_absolutePathInSamples = val; }
	void setMultipleConfigs( bool val ) { m_multipleConfigs = val; }
	bool multipleConfigs() { return m_multipleConfigs; }
	void setClearLogBeforeBuild( bool val ) { m_clearLogBeforeBuild = val; }
	bool clearLogBeforeBuild() { return m_clearLogBeforeBuild; }
	void setVerboseBuild( bool val ) { m_verboseBuild = val; }
	bool verboseBuild() { return m_verboseBuild; }

private:
	Settings();
	
	static void destroy();
	static QString findRelativePathDepthSearch( const QString& filename, const QDir& startPoint );

private:
	QString repositoryFile;
	QString m_configFileName;
	QString m_buildPath;
	QStringList m_includePath;
	QString m_installPath;
	QString m_sourceDir;
	QString m_appDir;
	QString m_toolPath;
	QString m_targetPlatform;
	QString m_programmer;
	bool m_absolutePathInSamples;
	bool m_multipleConfigs;
	bool m_clearLogBeforeBuild;
	bool m_verboseBuild;
};

#endif // __SETTINGS_H__
