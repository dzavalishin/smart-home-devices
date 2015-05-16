#include <stdlib.h>

#include "nutcomponentmodel.h"
#include "dirtraverser.h"
#include "settings.h"

#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QTextStream>

DirTraverser::DirTraverser()
{
}

DirTraverser::~DirTraverser()
{
	foreach( AbstractFileCopyFilter* filter, filters )
	{
		delete filter;
	}
	filters.clear();
}

void DirTraverser::copyDir( const QString& src, const QString& dest )
{
	//Check whether the dir directory exists
	if( QDir( src ).exists() ) 
	{
		if( !QDir( dest ).exists() ) 
			QDir().mkpath( dest );

		//Construct an iterator to get the entries in the directory
		QDirIterator dirIterator( src, QDir::AllEntries | QDir::NoDotAndDotDot );
		while ( dirIterator.hasNext() ) 
		{
			QString item = dirIterator.next();
			QString fileName = dirIterator.fileName();
			QFileInfo fileInfo = dirIterator.fileInfo();

			//If entry is a file copy it
			if ( fileInfo.isFile() )
			{
				bool done = false;

				foreach( AbstractFileCopyFilter* filter, filters ) {
					done = filter->onFile( fileInfo, dest );
					if ( done )
						break;
				}
				if ( !done )
					QFile::copy( item, dest + "/" + fileName );
			}
			else
				copyDir( item, dest + "/" + fileName );
		}
	}
	else
	{
		qWarning( qPrintable(QString("DirTraverser::copyDir - src folder [%1] doesn't exist").arg(src)) );
	}
}

void DirTraverser::insertFilter( AbstractFileCopyFilter* filter )
{
	filters.insert( 0, filter );
}

bool AppDirCopyFilter::onFile( const QFileInfo &fileInfo, const QString& dest)
{
	Q_UNUSED(dest);
	QString fileName = fileInfo.fileName();
	const char* exclude_pattern[] = {
		"Makeburn\\..*",
		"Makedefs\\..*",
		"Makerules\\..*",
		"Makevars\\..*"
	};

	for (int i = 0; i < sizeof(exclude_pattern) / sizeof(exclude_pattern[0]); i++)
		if ( QRegExp(exclude_pattern[i]).exactMatch(fileName) )
			return true;

	return false;
}

IccProjectCopyFilter::IccProjectCopyFilter( NutComponentModel* model )
{
	m_model = model;
}

bool IccProjectCopyFilter::onFile( const QFileInfo& fileInfo, const QString& dest)
{
	if (fileInfo.suffix().toLower() == "src") {
		QFile destFile( dest + "/" + fileInfo.fileName() );

		if (!destFile.exists()) {
			QFile srcFile( fileInfo.filePath() );

			if ( destFile.open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) ) {
				if ( srcFile.open( QFile::ReadOnly | QFile::Text ) ) {
					while (!srcFile.atEnd()) {
						QByteArray line;

						line = srcFile.readLine();
						if (line.startsWith("..\\..\\app\\"))
							line.remove(0, 10);
						destFile.write(line);
					}
					srcFile.close();
				}
				destFile.close();
			}
		}
	}
	if (fileInfo.suffix().toLower() == "prj") {
		QFile destFile( dest + "/" + fileInfo.fileName() );

		if (!destFile.exists()) {
			QFile srcFile( fileInfo.filePath() );

			if ( destFile.open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) ) {
				if ( srcFile.open( QFile::ReadOnly | QFile::Text ) ) {
					bool in_section = false;

					while (!srcFile.atEnd()) {
						QByteArray line;

						line = srcFile.readLine();
			            if (in_section) {
							QString item;

							if (line.startsWith("Edit1=")) {
								line.clear();
								/* If we have a first include path, add it first. */
								if (Settings::instance()->includePath().size() > 0) {
									item = Settings::instance()->includePath()[0];
									if (!item.isEmpty()) {
										line += item + ";";
									}
								}
								/* Add the include path of the build tree. */
								item = Settings::instance()->buildPath() + "/include";
								line += QDir(item).absolutePath() + ";";
								/* Add the ICC specific include path of the source tree. */
								item = Settings::instance()->sourceDir() + "/include/crt/iccavr";
								line += QDir(item).absolutePath() + ";";
								/* Add the include path of the source tree. */
								item = Settings::instance()->sourceDir() + "/include";
								line += QDir(item).absolutePath();
								/* If we have a last include path, add it last. */
								if (Settings::instance()->includePath().size() > 1) {
									item = Settings::instance()->includePath()[1];
									if (!item.isEmpty()) {
										line += ";" + item;
									}
								}
								line = "Edit1=" + line + "\n";
								line.replace("/", "\\");
							}
							else if (line.startsWith("Edit2=")) {
								item = Settings::instance()->buildPath() + "/lib";
								line = "Edit2=";
								line += QDir(item).absolutePath() + "\n";
								line.replace("/", "\\");
							}
							else if (line.startsWith("Edit3=")) {
								NUTCOMPONENTOPTION *opt = m_model->findOptionByName(NULL, "PLATFORM");
								line = "Edit3=";
								if (opt && opt->nco_enabled && opt->nco_active) {
									if (opt->nco_value && opt->nco_value[0]) {
										line += opt->nco_value;
										line += " ";
									} else {
										char *value = GetConfigValueOrDefault(m_model->repository(), opt->nco_compo, opt->nco_name);
										if (value) {
											line += value;
											line += " ";
											free(value);
										}
									}
								}
								opt = m_model->findOptionByName(NULL, "MCU_ATMEGA2561");
								if (opt == NULL) {
									opt = m_model->findOptionByName(NULL, "MCU_ATMEGA2560");
								}
								if (opt && opt->nco_enabled && opt->nco_active) {
									line += "_MCU_extended";
								}
								else {
									line += "_MCU_enhanced";
								}
								line += " __HARVARD_ARCH__";
								line += " ATMEGA";
								line += " CONST=\"\"\n";
							}
							else if (line.startsWith("Edit13=")) {
								char *value = NULL;
								NUTCOMPONENTOPTION *opt = m_model->findOptionByName(NULL, "ICCAVR_STARTUP");
								line = "Edit13=";

								if (opt && opt->nco_enabled && opt->nco_active) {
									if (opt->nco_value) {
										value = strdup(opt->nco_value);
									} else {
										value = GetConfigValueOrDefault(m_model->repository(), opt->nco_compo, opt->nco_name);
									}
								}
								QString valstr;
								if (value) {
									valstr = QString(value).trimmed();
									free(value);
								}
								if (valstr.isEmpty()) {
									line += "crtenutram";
								} else {
									line += valstr;
								}
								line += ".o\n";
							}
							else if (line.startsWith("Edit27=")) {
								item = Settings::instance()->buildPath() + "/lib/nutinit.o";
								line = "Edit27=";
								line += QDir(item).absolutePath() + "\n";
								line.replace("/", "\\");
							}
						}
						else if (line[0] == '[') {
							in_section = line.startsWith("[Compiler Options]");
						}
						destFile.write(line);
					}
					srcFile.close();
				}
				destFile.close();
			}
		}
	}
	return true;
}
