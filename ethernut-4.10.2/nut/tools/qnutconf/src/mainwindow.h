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

#if !defined( __MAINWINDOW_H__ )
#define __MAINWINDOW_H__

#include <QTime>
#include <QMainWindow>

#include "ui_mainwindow.h"

class FindDialog;
class NutComponentModel;
class NutComponentModelFilterProxy;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	Ui::MainWindow ui;
	QTime time;

public:
	MainWindow();
	~MainWindow();

public slots:
	void on_actionOpen_triggered();
	void on_actionSave_triggered();

	void on_actionSave_as_triggered();
	void on_actionExit_triggered();
	void on_actionFind_triggered();
	void on_actionSettings_triggered();
	void on_actionBuild_Nut_OS_triggered();
	void on_actionBuildStop_triggered();
	void on_actionCreate_sample_triggered();
	void on_actionAbout_triggered();

	void on_findNext_triggered(const QString &text);

private:
	void readSettings();
	void writeSettings();
	void updateWindowTitle();
	void generateApplicationTree();
	void saveConfig( QString filename = QString() );

private slots:
	void buildFinished( int exitCode );
	void updateView(const QModelIndex& current, const QModelIndex& previous);
	void message( const QString& );
	void resizeComponentTreeToContents();
	void documentModified();

private:
	NutComponentModel* model;
	NutComponentModelFilterProxy* proxyModel;
	FindDialog *m_findDialog;
	QString m_findText;
	QModelIndexList m_foundItems;
	int m_foundItemIndex;
};

#endif // __MAINWINDOW_H__
