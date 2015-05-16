
#include "nutcomponentmodel.h"
#include "nutcomponentmodel_p.h"
#include "nutcomponentdetailsmodel.h"

/*!
	\class NutComponentDetailsModel is a Proxy Model to NutComponentModel, presenting details of
	a given item using the refresh method.
*/

NutComponentDetailsModel::NutComponentDetailsModel( NutComponentModel* parent ) : QAbstractTableModel( parent ), parentModel( parent )
{
}

QVariant NutComponentDetailsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ ) const
{
	if ( orientation != Qt::Horizontal )
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	switch( section )
	{
	case 0:	return tr("Property");
	case 1: return tr("Value");
	default: break;
	}

	return QVariant();
}

QVariant NutComponentDetailsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole */ ) const
{
	if ( role == Qt::DisplayRole )
	{
		QStringList row = cache.at( index.row() );
		return row.at( index.column() );
	}
	return QVariant();
}

int NutComponentDetailsModel::rowCount( const QModelIndex& /*= QModelIndex() */ ) const
{
	return cache.count();
}

int NutComponentDetailsModel::columnCount( const QModelIndex& /*= QModelIndex() */ ) const
{
	return 2;
}

/*!
	Retrieve from parent model details for a given item.
*/
void NutComponentDetailsModel::refresh( const QModelIndex& selected )
{
#if QT_VERSION >= 0x040600
	beginResetModel();
#endif

	cache.clear();
	if ( !selected.isValid() )
		return;

	cache.append( QStringList() << tr("Enabled") << (selected.flags() & Qt::ItemIsEnabled ? tr("Yes") : tr("No")) );

	QString value;
	value = selected.data( NutComponentModel::Depends ).toStringList().join(", ");
	if ( !value.isEmpty() )
		cache.append( QStringList() << tr("Requires") << value );

	value = selected.data( NutComponentModel::Provides ).toStringList().join(", ");
	if ( !value.isEmpty() )
		cache.append( QStringList() << tr("Provides") << value );

	value = selected.data( NutComponentModel::File ).toString();
	if ( !value.isEmpty() )
		cache.append( QStringList() << tr("File") << value );

	value = selected.data( NutComponentModel::Macro ).toString();
	if ( !value.isEmpty() )
		cache.append( QStringList() << tr("Macro") << value );

	value = selected.data( NutComponentModel::Active ).toBool() ? tr("Yes") : tr("No");
	cache.append( QStringList() << tr("Active") << value );

#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
}
