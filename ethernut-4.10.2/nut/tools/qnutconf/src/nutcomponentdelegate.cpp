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

#include <QSpinBox>
#include <QComboBox>
#include <QApplication>

#include <limits>

#include "nutcomponentdelegate.h"
#include "nutcomponentmodel.h"
#include "nutcomponentmodel_p.h"


NutComponentDelegate::NutComponentDelegate( QObject* parent /* = 0 */ ) : QItemDelegate( parent )
{
	paintRadio = false;
}

QWidget* NutComponentDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
	Q_UNUSED(option);

	if (!index.isValid())
		return 0;
	
	switch ( index.data( NutComponentModel::OptionType ).toInt() )
	{
	case TreeItem::nutInteger:
		{
			QSpinBox* w = new QSpinBox( parent );
			w->setMaximum( std::numeric_limits<int>::max() );
			return w;
		}

	case TreeItem::nutEnumerated:
		return new QComboBox( parent );

	default:
		return QItemDelegate::createEditor( parent, option, index );
	}
}

void NutComponentDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
	Q_UNUSED(index);
	if ( editor->inherits("QSpinBox") )
	{
		QSpinBox* widget = qobject_cast<QSpinBox*>( editor );
		int value = index.model()->data( index, Qt::EditRole ).toInt();
		widget->setValue( value );
	}
	else if ( editor->inherits("QComboBox") )
	{
		QComboBox* widget = qobject_cast<QComboBox*>( editor );
		widget->addItems( index.data( NutComponentModel::OptionChoices ).toStringList() );
		widget->setCurrentIndex( widget->findText( index.data().toString() ) );
	}
	else
		QItemDelegate::setEditorData( editor, index );
}

void NutComponentDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
	if ( editor->inherits("QSpinBox") )
	{
		QSpinBox* widget = qobject_cast<QSpinBox*>( editor );
		widget->interpretText();
		int value = widget->value();
		model->setData( index, value, Qt::EditRole );
	}
	else if ( editor->inherits("QComboBox") )
	{
		QComboBox* widget = qobject_cast<QComboBox*>( editor );
		model->setData( index, widget->currentText(), Qt::EditRole );
	}
	else
		QItemDelegate::setModelData( editor, model, index );
}

void NutComponentDelegate::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}

void NutComponentDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

	if ( index.data( NutComponentModel::UI_Hint ) == TreeItem::nutHintRadio )
		paintRadio = true;

	QStyleOptionViewItem opt(option);
	if ( !( index.data(NutComponentModel::Enabled).toBool() ) ) {
		opt.state &= ~QStyle::State_Enabled;
	}		
	QItemDelegate::paint( painter, opt, index );

	paintRadio = false;
}

void NutComponentDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, Qt::CheckState state) const
{
	if (!rect.isValid())
		return;

	QStyleOptionViewItem opt(option);
	opt.rect = rect;
	opt.state = opt.state & ~QStyle::State_HasFocus;

	switch (state) {
	case Qt::Unchecked:
		opt.state |= QStyle::State_Off;
		break;
	case Qt::PartiallyChecked:
		opt.state |= QStyle::State_NoChange;
		break;
	case Qt::Checked:
		opt.state |= QStyle::State_On;
		break;
	}

	const QWidget *widget = 0;
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		widget = v3->widget;

	QStyle *style = widget ? widget->style() : QApplication::style();

	if ( paintRadio )
		style->drawPrimitive(QStyle::PE_IndicatorRadioButton, &opt, painter, widget);
	else
		style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, widget);
}

