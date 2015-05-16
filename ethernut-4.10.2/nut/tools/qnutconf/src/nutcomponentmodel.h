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

#if !defined( __NUTCOMPONENTTREEMODEL_H__ )
#define __NUTCOMPONENTTREEMODEL_H__

#include <QList>
#include <QVariant>
#include <QAbstractItemModel>

#include "nutcomponent.h"

class QTextStream;

class NutComponentModel : public QAbstractItemModel
{
	Q_OBJECT

	class NutComponentModelPrivate;
	NutComponentModelPrivate* d;

	friend class TreeItem;
	friend class NutComponentDelegate;

public:

	enum CustomRoles {
		Description = Qt::UserRole,
		Name,
		Depends,
		Provides,
		File,
		Macro,
		FullSearch,
		Enabled,
		Active,
		UI_Hint,
		OptionType,
		OptionChoices,
	};

	NutComponentModel(QObject *parent = 0);
	~NutComponentModel();

	bool openConfig( const QString& );
	bool saveConfig( const QString& filename );

	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	NUTREPOSITORY* repository();
	NUTCOMPONENTOPTION* findOptionByName(NUTCOMPONENT* compo, const char* name);

	// Build methods
	bool generateBuildTree();
	bool generateSampleMakefiles();

signals:
	void errorMessage( const QString& );
	void message(const QString&);
	void modified();

private:
	void close();
	void rebuildTree();
	bool isOptionActive( const char* );
	void deactivateOptionList(char **exlist, NUTCOMPONENT* compo = 0);

	void saveComponentOptions( QTextStream& file, NUTCOMPONENT* compo );
};

#endif //__NUTCOMPONENTTREEMODEL_H__
