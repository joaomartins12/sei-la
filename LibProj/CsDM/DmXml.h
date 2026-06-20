#pragma once

#ifndef DM_SYSTEM_XML_H
#define DM_SYSTEM_XML_H

#include "Ext/TinyXml/tinyxml.h"

namespace DmCS
{
	class Xml
	{
	public:
		class IXmlObject
		{
		public:
			virtual ~IXmlObject(void){}

			virtual void VChild(Xml conref sub) DmDecl;
			virtual void VAttribute(char cptr name, char cptr value) DmDecl;
		};

	public:
		Xml(void);
		Xml(TiXmlElement* node);
		Xml(Xml conref xml);
		Xml conref operator=(TiXmlElement* node);
		Xml conref operator=(Xml conref xml);

		char cptr Name(void) const;
		char cptr Attribute(char cptr name) const;
		char cptr Text(void) const;
		Xml NewChild(char cptr name) const;
		Xml Child(char cptr name) const;
		Xml FirstChild(void) const;
		void ClearChild(void) const;
		Xml Next(void) const;
		bool Eon(void) const;
		void ForeachChild(IXmlObject& object) const;
		void ForeachAttribute(IXmlObject& object) const;

		Xml InsertChild(Xml conref sub);
		void SetAttribute(char cptr name, char cptr value);

		//-------------------------------------------------
		//	기존파서와의 연계를 위해서
		operator TiXmlElement*(void) const;

	private:
		TiXmlAttribute cptr FirstAttribute(void) const;

	protected:
		TiXmlElement*	m_pElement;
	};

	class XmlFile
		: public Xml
	{
	public:
		XmlFile(void);
		~XmlFile(void);

		bool Open(char cptr file);
		bool Open(dm_bytevec conref datavec);
		bool Create(char cptr file, char cptr root);
		bool Root(char cptr root);
		bool Save(void);
		void Close(void);

	private:
		TiXmlDocument	m_XmlDoc;
	};
}

#endif//ECLIPSE_WAR_SYSTEM_XML_H
