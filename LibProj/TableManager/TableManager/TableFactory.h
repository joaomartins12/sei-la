
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
BEGIN_NAME_TABLE
namespace TFactory
{
	IDataTableEvent* CreateITable(int const& TableType);
	ITableEvent* CreateISTable(int const& TableType);
}
END_NAME_TABLE