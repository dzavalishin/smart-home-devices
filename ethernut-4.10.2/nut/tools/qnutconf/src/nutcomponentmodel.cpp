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

#include "nutcomponentmodel.h"

#include <QtGui>
#include <QFile>

#include <string.h>

#include "nutcomponentmodel.h"
#include "nutcomponentmodel_p.h"

#include "nutcomponent.h"
#include "settings.h"


class NutComponentModel::NutComponentModelPrivate
{
public:
	NutComponentModelPrivate() : rootItem( 0 ), repository(0), rootComponent(0)
	{

	}

	~NutComponentModelPrivate()
	{
		delete rootItem;
		if ( repository )
			CloseRepository( repository );
		if ( rootComponent )
			ReleaseComponents( rootComponent );
	}

	void addChildItems( NUTCOMPONENT* parentComponent, TreeItem* parentItem, NutComponentModel* model );

	TreeItem *rootItem;
	NUTREPOSITORY* repository;
	NUTCOMPONENT* rootComponent;
	QString repositoryPath;
};

void NutComponentModel::NutComponentModelPrivate::addChildItems( NUTCOMPONENT* parentComponent, TreeItem* parentItem, NutComponentModel* model )
{
	if(parentComponent) {
		parentComponent = parentComponent->nc_child;
		while (parentComponent) {
			TreeItem* item = new TreeItem(parentComponent, model, parentItem);
			parentItem->appendChild( item );

			NUTCOMPONENTOPTION *opts = parentComponent->nc_opts;
			while (opts) {
				TreeItem* childItem = new TreeItem(opts, model, item);
				item->appendChild( childItem );
				opts = opts->nco_nxt;
			}

			addChildItems(parentComponent, item, model);
			parentComponent = parentComponent->nc_nxt;
		}
	}

}

NutComponentModel::NutComponentModel(QObject *parent)
: QAbstractItemModel(parent), d( new NutComponentModelPrivate )
{
	d->rootItem = new TreeItem( 0, 0 );
}

NutComponentModel::~NutComponentModel()
{
	delete d;
}

bool NutComponentModel::openConfig( const QString& fileName )
{
	if ( d->repository )
		close();

	if ( !d->repository )
	{
		QString repositoryPath = Settings::instance()->repository();
		d->repositoryPath = QDir::fromNativeSeparators(repositoryPath);

		if ( !QFile::exists( d->repositoryPath ) )
		{
			emit errorMessage( tr("Couldn't find repository at [%1].\n Please check your settings.").arg(QDir::toNativeSeparators(d->repositoryPath)) );
			return false;
		}

		d->repository = OpenRepository( qPrintable(d->repositoryPath) );
		if ( !d->repository )
		{
			emit errorMessage( QLatin1String(GetScriptErrorString()) );
			return false; // Failed to open repository
		}

		d->rootComponent = LoadComponents(d->repository);
		if ( !d->rootComponent )
		{
			emit errorMessage( QLatin1String(GetScriptErrorString()) );
			return false; // Failed to open repository
		}

		if( ConfigureComponents(d->repository, d->rootComponent, qPrintable(fileName)) )
		{
			emit errorMessage( QLatin1String(GetScriptErrorString()) );
			return false; // Failed to open repository
		}

		if ( RefreshComponents(d->repository, d->rootComponent) )
			emit errorMessage( tr("WARNING: Cyclic Dependency Detected. Check dependencies on the .nut files") );
		 else
			emit message( tr("OK") );
	}
	rebuildTree();
	reset();
	return true;
}

void NutComponentModel::saveComponentOptions( QTextStream& stream, NUTCOMPONENT* compo )
{
	while( compo )
	{
		NUTCOMPONENTOPTION* opts = compo->nc_opts;
		while (opts)
		{
			if(opts->nco_enabled && opts->nco_active)
			{
				QString value;
				if (opts->nco_value)
				{
					/* Save edited value. */
					value = QLatin1String(opts->nco_value);
				}
				else
				{
					/* Save configured value. */
					char* v = GetConfigValue( d->repository, opts->nco_name );
					value = QLatin1String( v );
					free( v );
				}

				/* Do not save empty values, unless they are boolean. */
				if ( value.isEmpty() )
				{
					char* flavor = GetOptionFlavour(d->repository, opts->nco_compo, opts->nco_name);
					QString flavorString = QString(QLatin1String(flavor)).toLower();
					if ( flavor )
						free( flavor );
					if ( flavorString.startsWith( "bool" ) )
						stream << opts->nco_name << " = \"\"\n";
					else
						qWarning( qPrintable(QString("Not saving %1 flavor %2").arg(opts->nco_name).arg(flavorString)) );
				}
				else
				{
					value = value.replace("\"", "\\\""); // Escape (") to (\")
					stream << opts->nco_name << " = \"" << value << "\"\n";
				}
			}
			opts = opts->nco_nxt;
		}
		saveComponentOptions(stream, compo->nc_child);
		compo = compo->nc_nxt;
	}
}

bool NutComponentModel::saveConfig( const QString& filename )
{
	if ( filename.isEmpty() )
		return false;

	QFile file( filename );
	if ( file.open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) )
	{
		QTextStream stream( &file );
		saveComponentOptions( stream, d->rootComponent );
		file.close();
		return true;
	}
	return false;
}


/*!
	Close the repository and go back to a pre openConfig state
*/
void NutComponentModel::close()
{
	if ( d->repository )
	{
		CloseRepository( d->repository );
		d->repository = 0;
		d->repositoryPath.clear();
	}
	delete d->rootItem;
	d->rootItem = new TreeItem( 0, 0 );
}

int NutComponentModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
	else
		return d->rootItem->columnCount();
}

QVariant NutComponentModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	return item->data(index.column(), role);
}

bool NutComponentModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	if ( !index.isValid() )
		return false;

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	if ( role == Qt::EditRole )
	{
		if ( item->setValue( value ) )
		{
			emit dataChanged(index, index);
			emit modified();
			return true;
		}
	}
	else if ( role == Qt::CheckStateRole )
	{
		item->setActive( value.toBool() );
		if ( RefreshComponents(d->repository, d->rootComponent) )
			emit errorMessage( tr("WARNING: Cyclic Dependency Detected. Check dependencies on the .nut files") );

		// RefreshComponents might have changed the status on the whole tree, refresh everything.
		emit dataChanged( this->index(0, 0), this->index(rowCount(), 2) );
		emit modified();
		return true;
	}
	return false;
}

Qt::ItemFlags NutComponentModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	return item->flags(index.column());
}

QVariant NutComponentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}

QModelIndex NutComponentModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = d->rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex NutComponentModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem *parentItem = childItem->parent();

	if (parentItem == d->rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int NutComponentModel::rowCount(const QModelIndex &parent) const
{
	TreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = d->rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

void NutComponentModel::rebuildTree()
{
	d->rootItem->clear();
	d->addChildItems( d->rootComponent, d->rootItem, this );
}

NUTREPOSITORY* NutComponentModel::repository()
{
	return d->repository;
}

bool NutComponentModel::isOptionActive( const char* name )
{
	NUTCOMPONENTOPTION *opt = findOptionByName(d->rootComponent->nc_child, name);

	if (opt && opt->nco_active) {
		return true;
	}
	return false;

}

NUTCOMPONENTOPTION* NutComponentModel::findOptionByName( NUTCOMPONENT* compo, const char* name )
{
	if (!compo && d->rootComponent)
		compo = d->rootComponent->nc_child;

	while (compo)
	{
		NUTCOMPONENTOPTION* opts = compo->nc_opts;
		while (opts) {
			if (strcmp(opts->nco_name, name) == 0) {
				return opts;
			}
			opts = opts->nco_nxt;
		}

		if (compo->nc_child && (opts = findOptionByName(compo->nc_child, name)) != 0) {
			return opts;
		}
		compo = compo->nc_nxt;
	}
	return 0;
}

/*!
	Deactivate a list of options.
*/
void NutComponentModel::deactivateOptionList( char **exlist, NUTCOMPONENT* compo /*= 0*/ )
{
	if (!exlist)
		return;

	if (!compo)
		compo = d->rootComponent->nc_child;

	for (int i = 0; exlist[i]; ++i)
	{
		NUTCOMPONENTOPTION *opt = findOptionByName(compo, exlist[i]);
		if (opt)
			opt->nco_active = 0;
	}
}

/*!
	Create all configuration headers and Makefiles
	Returns true on success, and false otherwise.
*/
bool NutComponentModel::generateBuildTree()
{
	QString instDir = Settings::instance()->installPath();
	if (instDir.isEmpty()) {
		instDir = Settings::instance()->buildPath() + "/lib";
	}

	emit message( tr("Creating Makefiles for %1 in %2").arg(Settings::instance()->targetPlatform(), Settings::instance()->buildPath()) );

	if ( CreateMakeFiles( d->repository, d->rootComponent, Settings::instance()->buildPath().toLocal8Bit(),
		Settings::instance()->sourceDir().toLocal8Bit(), Settings::instance()->targetPlatform().toLocal8Bit(),
#ifdef Q_OS_WIN32
		Settings::instance()->includePath().join(";").toLocal8Bit(),
#else
		Settings::instance()->includePath().join(":").toLocal8Bit(),
#endif
		0, qPrintable(instDir)) )
	{
		return false;
	}

	emit message( tr("Creating header files in %1").arg(Settings::instance()->buildPath()) );
	if ( CreateHeaderFiles( d->repository, d->rootComponent, Settings::instance()->buildPath().toLocal8Bit()) )
		return false;

	return true;
}

/*!
	Create Makefiles for the samples directory
*/
bool NutComponentModel::generateSampleMakefiles()
{
	QString buildPath = Settings::instance()->buildPath();
	QString appDir = Settings::instance()->appDir();
	QString srcDir = Settings::instance()->sourceDir();
	QString instDir = Settings::instance()->installPath();
	QByteArray platform = Settings::instance()->targetPlatform().toLocal8Bit();
	QByteArray programmer = Settings::instance()->programmer().toLocal8Bit();

	if (instDir.isEmpty()) {
		instDir = buildPath + "/lib";
	}
	if ( Settings::instance()->absolutePathInSamples() )
	{
		buildPath = QDir(buildPath).absolutePath();
		appDir = QDir(appDir).absolutePath();
		srcDir = QDir(srcDir).absolutePath();
		instDir = QDir(instDir).absolutePath();
	}

	if( CreateSampleDirectory( d->repository, d->rootComponent, qPrintable(buildPath), qPrintable(appDir), qPrintable(srcDir),
		qPrintable(instDir), platform, programmer, 0, 0 ) )
			return false;
	return true;
}
