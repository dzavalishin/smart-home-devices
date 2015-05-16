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
#include "nutcomponentmodel_p.h"


TreeItem::TreeItem( NUTCOMPONENT* pComponent, NutComponentModel* parentModel, TreeItem* parent /*= 0 */ ) : parentItem( parent ),
  model( parentModel ), parentComponent( pComponent ), componentOptions( 0 )
{
	/* Libraries contain modules. */
	if ( parentComponent )
	{
		if (parentComponent->nc_child)
			componentTypeValue = nutLibrary;
		else
			componentTypeValue = nutModule;
	}
}

TreeItem::TreeItem( NUTCOMPONENTOPTION* opts, NutComponentModel* parentModel, TreeItem* parent ) : parentItem( parent ),
  model( parentModel ), parentComponent( 0 ), componentOptions( opts )
{
	componentTypeValue = nutOption;

	if (componentOptions && componentOptions->nco_exclusivity)
		uiHint = nutHintRadio;
	else
		uiHint = nutHintNone;
}

TreeItem::~TreeItem()
{
	clear();
}

void TreeItem::clear()
{
	qDeleteAll(childItems);
	childItems.clear();
}

void TreeItem::appendChild( TreeItem *child )
{
	childItems.append(child);
}

TreeItem * TreeItem::child( int row )
{
	return childItems.value(row);
}

int TreeItem::childCount() const
{
	return childItems.count();
}

int TreeItem::columnCount() const
{
	return 2;
}

QVariant TreeItem::data( int column, int role ) const
{
	switch( role )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		if ( column == 0 )
			return briefDescription();
		else
			return value();
		break;

	case Qt::DecorationRole:
		if ( column == 0 )
			return icon(isEnabled());
		break;

	case Qt::CheckStateRole:
		if ( hasBool() && column == 0 )
		{
			if ( isActive() )
				return Qt::Checked;
			else
				return Qt::Unchecked;
		}
		break;

	case Qt::ToolTipRole:
		if ( isEnabled() )
			return description();
		else
			return QString("Depends on: %1").arg( depends().join(", ") );
		break;

	case NutComponentModel::Active:
		return isActive();

	case NutComponentModel::Enabled: //Use internal enabled state instead of flags() 
		return isEnabled();          //so user can select the item and get the help and description.

	case NutComponentModel::Description:
		return description();

	case NutComponentModel::Name:
		return name();

	case NutComponentModel::Depends:
		return depends();

	case NutComponentModel::Provides:
		return provides();

	case NutComponentModel::File:
		return headerFile();
	
	case NutComponentModel::Macro: // When a file is specified, name is the preprocessor macro name
		if ( !headerFile().isEmpty() )
			return name();
		break;

	case NutComponentModel::FullSearch:
		return briefDescription()
				+ " " + value().toString()
				+ " " + name()
				+ " " + description();

	case NutComponentModel::UI_Hint:
		return optionUIHint();

	case NutComponentModel::OptionType:
		return optionType();

	case NutComponentModel::OptionChoices:
		return optionChoices();

	default:
		break;
	}
	return QVariant();
}

int TreeItem::row() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

TreeItem * TreeItem::parent()
{
	return parentItem;
}

QString TreeItem::name() const
{
	if ( componentOptions )
		return QLatin1String( componentOptions->nco_name );
	else if ( parentComponent )
		return QLatin1String( parentComponent->nc_name );
	return QString();
}

QString TreeItem::briefDescription() const
{
	QString str;
	char *brief;

	if (parentComponent) {
		brief = GetComponentBrief(model->repository(), parentComponent);
		if (brief) {
			str = QLatin1String(brief);
			free(brief);
		}
	} else if (componentOptions) {
		brief = GetOptionBrief(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
		if (brief) {
			str = QLatin1String(brief);
			free(brief);
		}
	} else {
		str = ("Nut/OS Components");
	}

	if (str.isEmpty())
		return name();

	return str;
}

QString TreeItem::description() const
{
	QString str;
	char *desc = NULL;
	if (parentComponent) {
		desc = GetComponentDescription(model->repository(), parentComponent);
		if (desc) {
			str = QLatin1String(desc);
			free(desc);
		}
	} else if (componentOptions) {
		desc = GetOptionDescription(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
		if (desc) {
			str = QLatin1String(desc);
			free(desc);
		}
	}

	return str;
}

QVariant TreeItem::value() const
{
	QString str;

	if (componentOptions) {
		/* First try the edited value. */
		if (componentOptions->nco_value) {
			str = QLatin1String(componentOptions->nco_value);
		} else {
			/* Get either the configured value or its default. */
			char *val = GetConfigValueOrDefault(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
			if (val) {
				str = QLatin1String(val);
				free(val);
			}
			/* Return the first enumerated item, if no configured or default value. */
			else if (optionType() == nutEnumerated) {
				char **choices = GetOptionChoices(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
				if (choices) {
					if (choices[0]) {
						str = QLatin1String(choices[0]);
					}
					for (int i = 0; choices[i]; i++) {
						free(choices[i]);
					}
					free(choices);
				}
			}
		}
	}
	return str;
}

/*!
	Set the value of a configuration item
*/
bool TreeItem::setValue( const QVariant& v )
{
	if (componentOptions)
	{
		if ( v != value() )
		{
			if ( componentOptions->nco_value )
				free( componentOptions->nco_value );
			componentOptions->nco_value = strdup( qPrintable(v.toString()) );
			componentOptions->nco_active = 1;
		}
	}
	return true;
}

Qt::ItemFlags TreeItem::flags( int column ) const
{
	Q_UNUSED(column);
	Qt::ItemFlags f(Qt::ItemIsEnabled);

	if ( column == 0 )
		f |= Qt::ItemIsSelectable;

	if ( canEdit() && column == 1 )
		f |= Qt::ItemIsEditable;

	if ( optionFlavor() == nutFlavorBool || optionFlavor() == nutFlavorBoolData )
	{
		f |= Qt::ItemIsUserCheckable;
	}

	return f;
}

bool TreeItem::isEnabled() const
{
	if (componentOptions)
		return componentOptions->nco_enabled != 0;
	else if (parentComponent)
		return parentComponent->nc_enabled != 0;

	return false;
}

TreeItem::nutOptionType TreeItem::optionType() const
{
	nutOptionType rc = nutOptionTypeNone;

	if (componentOptions) {
		rc = nutString;
		char *type = GetOptionTypeString(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
		if (type) {
			QString typeString = QString(QLatin1String(type)).toLower();
			free(type);
			if (typeString == "integer") {
				rc = nutInteger;
			}
			else if (typeString == "bool") {
				rc = nutBool;
			}
			else if (typeString == "enumerated") {
				rc = nutEnumerated;
			}
		}
	}
	return rc;
}

TreeItem::nutComponentType TreeItem::componentType() const
{
	return componentTypeValue;
}

TreeItem::nutOptionFlavor TreeItem::optionFlavor() const
{
	if (componentOptions) {
		char *flavor = GetOptionFlavour(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
		if (flavor) {
			QString flavorString = QString(QLatin1String(flavor)).toLower();
			free(flavor);
			if (flavorString == "boolean")
				return nutFlavorBool;

			if (flavorString == "booldata")
				return nutFlavorBoolData;
		}
		return nutFlavorData;
	}
	return nutFlavorNone;
}

QPixmap TreeItem::icon(bool enabled) const
{
	switch( componentType() )
	{
	case nutLibrary:
		return QPixmap(":/images/library.png");

	case nutModule:
		if (enabled)
			return QPixmap(":/images/module.png");
		return QPixmap(":/images/module_dis.png");

	case nutOption:
		// Don't show up icon if there is a check box.
		if ( optionFlavor() != nutFlavorData )
			break;

		switch( optionType() )
		{
		case TreeItem::nutInteger:
			if (enabled)
				return QPixmap(":/images/integer.png");
			return QPixmap(":/images/integer_dis.png");

		case TreeItem::nutEnumerated:
			if (enabled)
				return QPixmap(":/images/enumerated.png");
			return QPixmap(":/images/enumerated_dis.png");
		case TreeItem::nutString:
			if (enabled)
				return QPixmap(":/images/text.png");
			return QPixmap(":/images/text_dis.png");
		default:
			break;
		}
	default:
		break;
	}
	return QPixmap();
}

bool TreeItem::canEdit() const
{
	if (!isEnabled())
		return false;

	if (componentType() != nutOption)
		return false;

	if (optionFlavor() != nutFlavorData && optionFlavor() != nutFlavorBoolData)
		return false;

	return true;
}

/*!
	Returns true if an option has been enabled by the user or
	automatically by the system.
	Only options can be active, components are always active.
*/
bool TreeItem::isActive() const
{
	if (componentOptions) {
		if (hasBool()) {
			if(componentOptions->nco_active == 0) {
				if(componentOptions->nco_active_if) {
					componentOptions->nco_active = model->isOptionActive(componentOptions->nco_active_if);
				}
			}
			return componentOptions->nco_active != 0;
		}
		componentOptions->nco_active = 1;
	}
	return true;
}

bool TreeItem::hasBool() const
{
	if (componentOptions) {
		char *flavor = GetOptionFlavour(model->repository(), componentOptions->nco_compo, componentOptions->nco_name);
		if (flavor) {
			QString flavorString = QString(QLatin1String(flavor)).toLower();
			free(flavor);
			if ( flavorString.startsWith("bool") ) // "boolean" || "booldata"
				return true;
		}
	}
	return false;
}

void TreeItem::setActive( bool enable )
{
	if (componentOptions)
	{
		// Make sure exclusive options only have one of them enabled.
		if ( enable && componentOptions->nco_exclusivity )
			model->deactivateOptionList(componentOptions->nco_exclusivity, componentOptions->nco_compo);

		componentOptions->nco_active = enable;
		if (enable && !componentOptions->nco_value)
			componentOptions->nco_value = strdup("");
	}
}

QStringList TreeItem::optionChoices() const
{
	char** choices = ::GetOptionChoices( model->repository(), componentOptions->nco_compo, componentOptions->nco_name );
	char** it = choices;
	QStringList result;

	while( *it )
		result.append( QLatin1String(*it++) );

	ReleaseStringArray( choices );
	return result;
}

TreeItem::nutUIHint TreeItem::optionUIHint() const
{
	return uiHint;
}

QStringList TreeItem::depends() const
{
	QStringList result;

	if (componentOptions)
	{
		char** requires = GetOptionRequirements( model->repository(), componentOptions->nco_compo, componentOptions->nco_name );
		char** it = requires;
		if ( it )
		{
			while ( *it )
				result.append( QLatin1String(*it++) );

			ReleaseStringArray( requires );
		}
	}

	/* Do we need component requirements? */
	if ( parentComponent )
	{
		char** requires = GetComponentRequirements( model->repository(), parentComponent );
		char** it = requires;

		if ( it )
		{
			while ( *it )
				result.append( QLatin1String(*it++) );

			ReleaseStringArray( requires );
		}
	}

	return result;
}

QStringList TreeItem::provides() const
{
	QStringList result;

	if (componentOptions)
	{
		char** requires = GetOptionProvisions( model->repository(), componentOptions->nco_compo, componentOptions->nco_name );
		char** it = requires;
		if ( it )
		{
			while ( *it )
				result.append( QLatin1String(*it++) );

			ReleaseStringArray( requires );
		}
	}

	/* Do we need component requirements? */
	if ( parentComponent )
	{
		char** requires = GetComponentProvisions( model->repository(), parentComponent );
		char** it = requires;

		if ( it )
		{
			while ( *it )
				result.append( QLatin1String(*it++) );

			ReleaseStringArray( requires );
		}
	}

	return result;
}

/*!
	Nut/OS is configured using auto-generated header files.
	This method returns the header file that this specific option is associated with.
*/
QString TreeItem::headerFile() const
{
	QString value;
	if (componentOptions)
	{
		char* filename = GetOptionFile( model->repository(), componentOptions->nco_compo, componentOptions->nco_name );
		if ( filename )
		{
			value = QLatin1String( filename );
			free(filename);
		}
	}
	return value;
}
