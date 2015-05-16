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

#if !defined( __NUTCOMPONENTTREEMODEL_P_H__ )
#define __NUTCOMPONENTTREEMODEL_P_H__

#include <QList>
#include <QVariant>
#include <QAbstractItemModel>
#include <QPixmap>

#include "nutcomponent.h"
#include "nutcomponentmodel.h"

class TreeItem
{
public:

	/*!
		Option types.
	*/
	enum nutOptionType {
		nutOptionTypeNone,
		nutInteger,
		nutBool,
		nutString,
		nutEnumerated
	};

	enum nutComponentType {
		nutConfigTypeNone,
		nutFolder,
		nutLibrary,
		nutModule,
		nutOption
	};

	enum nutOptionFlavor {
		nutFlavorNone,
		nutFlavorBool,
		nutFlavorBoolData,
		nutFlavorData
	};

	enum nutUIHint {
		nutHintNone,
		nutHintCheck,
		nutHintRadio
	};

	TreeItem( NUTCOMPONENT* pComponent, NutComponentModel* parentModel, TreeItem* parent = 0 );
	TreeItem( NUTCOMPONENTOPTION *opts, NutComponentModel* parentModel, TreeItem* parent );

	~TreeItem();

	void clear();
	void appendChild(TreeItem *child);
	TreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column, int role) const;
	Qt::ItemFlags flags(int column) const;
	int row() const;
	TreeItem *parent();
	QPixmap icon(bool enabled) const;

	QString name() const;
	QString description() const;
	QString briefDescription() const;
	bool isEnabled() const;
	bool canEdit() const;
	bool hasBool() const;
	QVariant value() const;
	bool setValue( const QVariant& );
	nutOptionType optionType() const;
	nutComponentType componentType() const;
	nutOptionFlavor optionFlavor() const;
	nutUIHint optionUIHint() const;

	QStringList optionChoices() const;

	bool isActive() const;
	void setActive( bool );
	QStringList depends() const;
	QStringList provides() const;
	QString headerFile() const;

private:
	QList<TreeItem*> childItems;
	QVariant itemData;
	TreeItem *parentItem;
	NutComponentModel* model;
	NUTCOMPONENT* parentComponent;
	NUTCOMPONENTOPTION* componentOptions;
	nutComponentType componentTypeValue;
	nutUIHint uiHint;
};

#endif //__NUTCOMPONENTTREEMODEL_P_H__
