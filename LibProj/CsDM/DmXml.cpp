#include "stdafx.h"
#include "DmXml.h"

namespace DmCS
{
	namespace xml_def
	{
		char cptr const version	= "1.0";
		char cptr const utf_8	= "UTF-8";
	}

	Xml::Xml(void)
		: m_pElement( 0 )
	{}
	
	Xml::Xml(TiXmlElement* node)
		: m_pElement( node )
	{}
	
	Xml::Xml(Xml conref xml) 
		: m_pElement( xml.m_pElement )
	{}
	
	Xml conref Xml::operator=(TiXmlElement* node)
	{
		m_pElement = node;
		return *this;
	}

	Xml conref Xml::operator=(Xml conref xml)
	{
		m_pElement = xml.m_pElement;
		return *this;
	}

	char cptr Xml::Name(void) const
	{
		return m_pElement->Value();
	}

	char cptr Xml::Attribute(char cptr name) const
	{
		return m_pElement->Attribute( name );
	}

	char cptr Xml::Text(void) const
	{
		return m_pElement->GetText();
	}

	Xml Xml::NewChild(char cptr name) const
	{
		Xml tmp;
		if( !m_pElement )
			return tmp;

		TiXmlNode* pNode = m_pElement->LinkEndChild( new TiXmlElement( name ) );
		tmp = pNode->ToElement();
		return tmp;
	}

	Xml Xml::Child(char cptr name) const
	{
		Xml tmp;
		if( !m_pElement )
			return tmp;

		tmp = m_pElement->FirstChildElement( name );
		if( tmp.Eon() )
			tmp = NewChild( name );

		return tmp;
	}

	Xml Xml::FirstChild(void) const
	{
		Xml tmp;
		if( !m_pElement )
			return tmp;

		tmp = m_pElement->FirstChildElement();
		return tmp;
	}

	void Xml::ClearChild(void) const
	{
		if( !m_pElement )
			return;

		m_pElement->Clear();
	}

	Xml Xml::Next(void) const
	{
		Xml tmp;
		if( !m_pElement )
			return tmp;

		tmp = m_pElement->NextSiblingElement();
		return tmp;
	}

	bool Xml::Eon(void) const
	{
		if( m_pElement )
			return false;

		return true;
	}

	void Xml::ForeachChild(IXmlObject& object) const
	{
		Xml sub = FirstChild();
		while( !sub.Eon() )
		{
			object.VChild( sub );
			sub = sub.Next();
		}
	}

	void Xml::ForeachAttribute(IXmlObject& object) const
	{
		TiXmlAttribute cptr pAttr = FirstAttribute();
		while( pAttr )
		{
			object.VAttribute( pAttr->Name(), pAttr->Value() );
			pAttr = pAttr->Next();
		}
	}

	Xml Xml::InsertChild(Xml conref sub)
	{
		Xml tmp;

		if( !m_pElement )
			return tmp;

		TiXmlNode* pAttached = m_pElement->InsertEndChild( *sub.m_pElement );
		tmp.m_pElement = pAttached->ToElement();
		return tmp;
	}

	void Xml::SetAttribute(char cptr mzName, char cptr mzValue)
	{
		m_pElement->SetAttribute( mzName, mzValue );
	}

	Xml::operator TiXmlElement*(void) const
	{
		return m_pElement;
	}

	TiXmlAttribute cptr Xml::FirstAttribute(void) const
	{
		if( !m_pElement )
			return 0;

		return m_pElement->FirstAttribute();
	}

	XmlFile::XmlFile(void)
		: Xml()
	{
	}

	XmlFile::~XmlFile(void)
	{
		Close();
	}

	bool XmlFile::Open(char cptr file)
	{
		Close();
		if( !m_XmlDoc.LoadFile( file ) )
			return false;

		return true;
	}

	bool XmlFile::Open(dm_bytevec conref datavec)
	{
		Close();
		m_XmlDoc.Parse( ( char const* )&datavec.at( 0 ) );
		if( m_XmlDoc.Error() )
			return false;

		return true;
	}

	bool XmlFile::Create(char cptr file, char cptr root)
	{
		Close();

		if( !m_XmlDoc.LoadFile( file ) )
		{
			if( !m_XmlDoc.LinkEndChild( new TiXmlDeclaration( xml_def::version, xml_def::utf_8, "" ) ) )
				return false;

			TiXmlNode* pNode = m_XmlDoc.LinkEndChild( new TiXmlElement( root ) );
			if( !pNode )
				return false;
		}

		Root( root );

		return Save();
	}

	bool XmlFile::Root(char cptr root)
	{
		m_pElement = m_XmlDoc.FirstChildElement( root );
		if( !m_pElement )
			return false;

		return true;
	}

	bool XmlFile::Save(void)
	{
		return m_XmlDoc.SaveFile();
	}

	void XmlFile::Close(void)
	{
		m_XmlDoc.Clear();
		m_pElement = 0;
	}
}