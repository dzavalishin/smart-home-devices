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

#include "settingsdialog.h"
#include "settings.h"

#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	// Repository tab
	ui.e_RepositoryFile->setText( Settings::instance()->repository() );
	ui.cb_MultiConfig->setChecked( Settings::instance()->multipleConfigs() );

	// Build tab
	ui.cb_ClearLogBeforeBuild->setChecked( Settings::instance()->clearLogBeforeBuild() );
	ui.cb_LogVerbose->setChecked( Settings::instance()->verboseBuild() );
	ui.e_SourceDirectory->setText( Settings::instance()->sourceDir() );
	ui.e_BuildDirectory->setText( Settings::instance()->buildPath() );
	ui.e_InstallDirectory->setText( Settings::instance()->installPath() );
	populatePlatform();

	// Tools tab
	ui.e_ToolPath->setText( Settings::instance()->toolPath() );

	// Samples tab
	ui.e_AppDirectory->setText( Settings::instance()->appDir() );
	ui.cb_AbsolutePathInSamples->setChecked( Settings::instance()->absolutePathInSamples() );
	populateProgrammer();
}

SettingsDialog::~SettingsDialog()
{

}

/*!
	Fills the platform selection combo box.
	Scans the source directory for files with a base name of 'Makedefs'. 
	The extensions of all files found are added to the combo box.
*/
void SettingsDialog::populatePlatform()
{
	QDir src_dir( ui.e_SourceDirectory->text() );
	QString platform = ui.cb_Platform->currentText();

	if ( src_dir.exists() )
	{
		ui.cb_Platform->clear();
		foreach( QString file, src_dir.entryList( QStringList() << "Makedefs.*") )
		{
			ui.cb_Platform->addItem( file.mid( 9 ) );
		}
	}

	if ( platform.isEmpty() )
		platform = Settings::instance()->targetPlatform();

	int index = ui.cb_Platform->findText( platform );
	if ( index >= 0 )
		ui.cb_Platform->setCurrentIndex( index );
}

/*!
	Fills the programmer selection combo box.
	Scans the <source directory>/app for files with a base name of 'Makeburn'.
	The extensions of all files found are added to the combo box.
*/
void SettingsDialog::populateProgrammer()
{
	QDir src_dir( ui.e_SourceDirectory->text() + QDir::separator() + "app" );
	QString programmer = ui.cb_Programmer->currentText();

	if ( src_dir.exists() )
	{
		ui.cb_Programmer->clear();
		foreach( QString file, src_dir.entryList( QStringList() << "Makeburn.*") )
		{
			ui.cb_Programmer->addItem( file.mid( 9 ) );
		}
	}
	
	if ( programmer.isEmpty() )
		programmer = Settings::instance()->programmer();

	int index = ui.cb_Programmer->findText( programmer );
	if ( index >= 0 )
		ui.cb_Programmer->setCurrentIndex( index );
}

void SettingsDialog::accept()
{
	// Update the Settings singleton.

	// Repository tab
	Settings::instance()->setRepository( ui.e_RepositoryFile->text() );
	Settings::instance()->setMultipleConfigs( ui.cb_MultiConfig->checkState() == Qt::Checked );

	// Build tab
	Settings::instance()->setClearLogBeforeBuild( ui.cb_ClearLogBeforeBuild->checkState() == Qt::Checked );
	Settings::instance()->setVerboseBuild( ui.cb_LogVerbose->checkState() == Qt::Checked );
	Settings::instance()->setSourceDir( ui.e_SourceDirectory->text() );
	Settings::instance()->setBuildPath( ui.e_BuildDirectory->text() );
	Settings::instance()->setInstallPath( ui.e_InstallDirectory->text() );
	Settings::instance()->setTargetPlatform( ui.cb_Platform->currentText() );

	// Tools tab
	Settings::instance()->setToolPath( ui.e_ToolPath->text() );

	// Samples tab
	Settings::instance()->setAppDir( ui.e_AppDirectory->text() );
	Settings::instance()->setProgrammer( ui.cb_Programmer->currentText() );
	Settings::instance()->setAbsolutePathInSamples( ui.cb_AbsolutePathInSamples->checkState() == Qt::Checked  );

	Settings::instance()->save();

	QDialog::accept();
}

/*!
	Looks for dynamic property "browseTarget" (string) in the slot sender, with the
	name of a QLineEdit, to fill with the user selected folder path.
	This dynamic property is usually set from the Qt Designer.
*/
void SettingsDialog::browseFolder()
{
	QObject* sender = QObject::sender();
	QVariant browseTarget = sender->property("browseTarget");
	if ( !browseTarget.isValid() )
		return;

	QLineEdit* target = findChild<QLineEdit*>( browseTarget.toString() );
	if ( !target )
		return;

	QString folder = QFileDialog::getExistingDirectory( this, tr("Browse"), target->text() );
	if ( !folder.isEmpty() )
		target->setText( folder );
}

/*!
	Looks for dynamic property "browseTarget" (string) in the slot sender, with the
	name of a QLineEdit, to fill with the user selected file path.
	If the optional "fileFilter" property is available, it will be used as the contents
	of the filter parameter to open the dialog.
	These dynamic properties are usually set from the Qt Designer.
*/
void SettingsDialog::browseFile()
{
	QObject* sender = QObject::sender();
	QVariant browseTarget = sender->property("browseTarget");
	QVariant filter = sender->property("fileFilter");
	if ( !browseTarget.isValid() )
		return;

	QLineEdit* target = findChild<QLineEdit*>( browseTarget.toString() );
	if ( !target )
		return;

	QString fileName = QFileDialog::getOpenFileName( this, tr("Browse"), target->text(), filter.toString() );
	if ( !fileName.isEmpty() )
		target->setText( fileName );
}

