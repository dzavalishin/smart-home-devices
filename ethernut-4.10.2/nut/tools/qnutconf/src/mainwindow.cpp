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

#include <QTime>
#include <QTimer>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>

#include "mainwindow.h"
#include "finddialog.h"
#include "settingsdialog.h"
#include "nutcomponentmodel.h"
#include "nutcomponentmodelfilterproxy.h"
#include "nutcomponentdetailsmodel.h"
#include "nutcomponentdelegate.h"
#include "settings.h"
#include "builder.h"
#include "dirtraverser.h"
#include "systeminfo.h"

// Transform a C Macro into a string expanding it
#define QUOTE_P(str) #str
#define QUOTE(str) QUOTE_P(str)

MainWindow::MainWindow()
{
	ui.setupUi( this );
	Settings::instance()->load();
	model = new NutComponentModel( this );
	m_findDialog = 0;

	proxyModel = new NutComponentModelFilterProxy( this );
	proxyModel->setSourceModel( model );

	ui.componentTree->setModel( proxyModel );
	ui.componentTree->setItemDelegate( new NutComponentDelegate( this ) );

	NutComponentDetailsModel* detailsModel = new NutComponentDetailsModel( model );
	ui.detailsView->setModel( detailsModel );

	connect( model, SIGNAL(errorMessage(const QString&)), SLOT(message(const QString&)) );
	connect( model, SIGNAL(message(const QString&)), SLOT(message(const QString&)) );
	connect( model, SIGNAL(modified()), SLOT(documentModified()) );
	connect( ui.componentTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(updateView(const QModelIndex&, const QModelIndex&)) );
	connect( ui.componentTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), detailsModel, SLOT(refresh(const QModelIndex&)) );
	connect( ui.componentTree, SIGNAL(expanded(const QModelIndex&)), SLOT(resizeComponentTreeToContents()));

	connect( ui.actionViewComponentTreeDisabledItems, SIGNAL(triggered(bool)), proxyModel, SLOT(showDisabledItems(bool)) );

	readSettings();

	message( tr("Nut/OS Configurator Version %1").arg(QUOTE(NUTCONF_VERSION)) );
	message( tr("Linked to Qt %1, running on %2").arg(QT_VERSION_STR, SystemInfo::GetOsString()) );
	message( tr("Working in %1").arg( QDir::toNativeSeparators( QDir::current().absolutePath() ) ) );

	connect( Builder::instance(), SIGNAL(message(const QString&)), SLOT(message(const QString&)) );
	connect( Builder::instance(), SIGNAL(done(int)), SLOT(buildFinished(int)) );

	// Run File->Open as soon as we enter the event loop
	QTimer::singleShot( 100, this, SLOT(on_actionOpen_triggered()) );
}

MainWindow::~MainWindow()
{
	delete m_findDialog;
	writeSettings();
}

void MainWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("pos", QPoint(50, 50)).toPoint();
	QSize size = settings.value("size", QSize(550, 350)).toSize();
	bool showDisabledItems = settings.value("showDisabledItems", false).toBool();
	resize(size);
	move(pos);
	proxyModel->showDisabledItems( showDisabledItems );
	ui.actionViewComponentTreeDisabledItems->setChecked( showDisabledItems );
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("showDisabledItems", ui.actionViewComponentTreeDisabledItems->isChecked());
}

void MainWindow::on_actionOpen_triggered()
{
	// Retrieve conf directory visited last.
	QSettings settings;
	QString confdir = settings.value("confpath", Settings::instance()->sourceDir() + "/conf").toString();

	QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), confdir, tr("Nut/OS Configuration (*.conf)") );
	if ( !fileName.isEmpty() )
	{
		// Save conf directory for later retrieval.
		confdir = QFileInfo(fileName).absolutePath();
		settings.setValue("confpath", confdir);

		QApplication::setOverrideCursor( Qt::BusyCursor );
		if ( !model->openConfig( fileName ) )
		{
			QApplication::restoreOverrideCursor();
			QMessageBox::critical(this, tr("Erro"), tr("There was a problem opening the file, please check the log message"));
		}
		else
		{
			Settings::instance()->load( fileName );
			setWindowFilePath( fileName );
			updateWindowTitle();
			ui.componentTree->resizeColumnToContents( 0 );
		}
		QApplication::restoreOverrideCursor();
	}
}

void MainWindow::saveConfig( QString filename /* = QString() */ )
{
	if ( filename.isEmpty() )
	{
		if ( Settings::instance()->configFileName().isEmpty() )
		{
			filename = QFileDialog::getSaveFileName( this, tr("Save as..."), QString(), tr("Nut/OS Configuration (*.conf)") );
			if ( filename.isEmpty() ) // User canceled
				return;
		}
		else
			filename = Settings::instance()->configFileName();
	}
	Settings::instance()->setConfigFileName( filename );
	model->saveConfig( filename );
	setWindowFilePath( filename );
	setWindowModified( false );
	updateWindowTitle();
	statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::on_actionSave_triggered()
{
	saveConfig( Settings::instance()->configFileName() );
}

void MainWindow::on_actionSave_as_triggered()
{
	QString fileName = QFileDialog::getSaveFileName( this, tr("Save as..."), Settings::instance()->configFileName(), tr("Nut/OS Configuration (*.conf)") );
	if ( fileName.isEmpty() )
		return;

	saveConfig( fileName );
}

void MainWindow::on_actionExit_triggered()
{
	if ( isWindowModified() )
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, QString(),
			                             tr("The document has been modified.\n"
			                                "Do you want to save your changes?"),
			                             QMessageBox::Save | QMessageBox::Discard
			                             | QMessageBox::Cancel);
		if (ret == QMessageBox::Save)
			saveConfig();
		else if (ret == QMessageBox::Cancel)
			return;
	}

	QApplication::quit();
}

void MainWindow::on_actionFind_triggered()
{
	if (m_findDialog == 0) {
		m_findDialog = new FindDialog(this);
		connect(m_findDialog, SIGNAL(findNext(const QString &)), this, SLOT(on_findNext_triggered(const QString &)));
	}
	m_findDialog->show();
	m_findDialog->raise();
	m_findDialog->activateWindow();
}

void MainWindow::on_findNext_triggered(const QString &text)
{
	if (m_findText != text || m_foundItems.isEmpty()) {
		QApplication::setOverrideCursor(Qt::WaitCursor);

		m_foundItems = model->match(model->index(0, 0), NutComponentModel::FullSearch, QVariant::fromValue(text), -1, 
			Qt::MatchFixedString | Qt::MatchContains | Qt::MatchRecursive | Qt::MatchWrap);

		QApplication::restoreOverrideCursor();
		m_findText = text;
		m_foundItemIndex = 0;
	} else {
		if (++m_foundItemIndex >= m_foundItems.count()) {
			m_foundItemIndex = 0;
		}
	}
	if (m_foundItems.isEmpty()) {
		QMessageBox::warning(this, tr("Search result"), tr("No items found"));
	} else {
		ui.componentTree->scrollTo(m_foundItems.at(m_foundItemIndex), QAbstractItemView::PositionAtCenter);
		ui.componentTree->setCurrentIndex(m_foundItems.at(m_foundItemIndex));
		activateWindow();
	}
}

void MainWindow::on_actionSettings_triggered()
{
	SettingsDialog dialog;
	dialog.exec();
}

void MainWindow::on_actionBuild_Nut_OS_triggered()
{
	/*
	 * Create a message box containing all relevant information and
	 * let the user decide if he really wants that.
	 */
	QString question = tr(
							"\nBuild directory:   %1"
							"\nTarget platform:   %2"
							"\nInstall directory: %3"
							"\n\nDo you want to build the Nut/OS libraries?\n"
							).arg( Settings::instance()->buildPath(), Settings::instance()->targetPlatform(), Settings::instance()->installPath() );
	if ( QMessageBox::question( this, tr("Build Nut/OS"), question, QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
		return; 

	// Disable build options.
	ui.actionBuild_Nut_OS->setEnabled( false );
	ui.actionCreate_sample->setEnabled( false );
	ui.actionBuildStop->setEnabled( true );

	// Clear log window
	if ( Settings::instance()->clearLogBeforeBuild() )
		ui.logPanel->clear();

	// Measure build time
	time.start();

	// Generate the build tree
	model->generateBuildTree();

	/* Clean up any previous build */
	Builder::instance()->build( "clean", Settings::instance()->verboseBuild() );
	/* Make all */
	Builder::instance()->build( "all", Settings::instance()->verboseBuild() );
	/* Make install */
	Builder::instance()->build( "install", Settings::instance()->verboseBuild() );

}

void MainWindow::on_actionBuildStop_triggered()
{
	Builder::instance()->stop();
}

void MainWindow::on_actionCreate_sample_triggered()
{
	generateApplicationTree();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, tr("Nut/OS Configurator"),
		tr("<p>Nut/OS Configurator version %1</p>"
		"<p>Visit the project <a href=\"http://www.ethernut.de\">website</a> for more information</p>" ).arg(QUOTE(NUTCONF_VERSION)));
}

void MainWindow::buildFinished( int exitCode )
{
	if ( !exitCode )
		message( tr("Build finished successfully in %1s" ).arg( time.elapsed() / 1000 ) );
	else
		message( tr("Build failed") );

	ui.actionBuild_Nut_OS->setEnabled( true );
	ui.actionCreate_sample->setEnabled( true );
	ui.actionBuildStop->setEnabled( false );
}

void MainWindow::updateView( const QModelIndex& current, const QModelIndex& previous )
{
	Q_UNUSED(previous);

	QVariant data = current.data( NutComponentModel::Description );
	ui.descriptionPanel->setText( data.toString() );
}

void MainWindow::message( const QString& msg )
{
	ui.logPanel->append( QTime::currentTime().toString("HH:mm:ss: ") +  msg );
}

void MainWindow::generateApplicationTree()
{
	QApplication::setOverrideCursor( Qt::BusyCursor );

	QString srcDir = Settings::instance()->sourceDir() + "/app";
	QString appDir = Settings::instance()->appDir();

	message( tr("Copying samples from %1 to %2").arg(srcDir, appDir) );
	DirTraverser appDirTraverser;
	appDirTraverser.insertFilter(new AppDirCopyFilter);
	appDirTraverser.copyDir( srcDir, appDir );

#ifdef Q_OS_WIN32
	srcDir = Settings::instance()->sourceDir() + "/appicc";

	message( tr("Translating ICCAVR projects from %1 to %2").arg(srcDir, appDir) );
	DirTraverser iccPrjTraverser;
	iccPrjTraverser.insertFilter(new IccProjectCopyFilter(model) );
	iccPrjTraverser.copyDir( srcDir, appDir );
#endif

	message( tr("Creating Makefiles for %1 in %2").arg(Settings::instance()->targetPlatform(), Settings::instance()->appDir()) );
	model->generateSampleMakefiles();

	message( tr("OK") );

	QApplication::restoreOverrideCursor();
}

void MainWindow::resizeComponentTreeToContents()
{
	ui.componentTree->resizeColumnToContents( 0 );
}

void MainWindow::updateWindowTitle()
{
	QString title = tr("Nut/OS Configurator");
	if ( !windowFilePath().isEmpty() )
	{
		title += " - " + QFileInfo( windowFilePath() ).fileName();
	}

	if ( isWindowModified() )
		title += "*";

	setWindowTitle( title );
}

/*!
	This slot is called whenever one of the settings in the component
	tree is changed.
*/
void MainWindow::documentModified()
{
	setWindowModified(true);
	updateWindowTitle();
}

