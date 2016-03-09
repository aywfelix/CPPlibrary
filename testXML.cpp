#include <iostream>
#include <string>
#include "xmlparser.h"
#include <stdio.h>
#include <string.h>
using namespace std;

class CXml
{
public:
    CXml(const string& xmlpath);
	CXml(const XMLNode& xmlNode);
    virtual ~CXml(){}
	string getString(const string& strXml);
	int getInteger(const string& strXml);
	bool getBoolean(const string& strXml);
	bool updateNode(const string& strXml, const string& value);

	XMLNode getRoot() { return xmlRoot; }
private:
	XMLNode getXmlNode(const string& strXml);
	bool LoadXml(const XMLNode& xmlNode);
	CXml(const CXml&);
	CXml& operator=(const CXml&);
	XMLNode xmlRoot;
	string xmlFile;
};

CXml::CXml(const string& xmlpath):xmlFile(xmlpath)
{
	XMLNode node = XMLNode::parseFile(xmlpath.c_str());
	if(node.isEmpty())
	{
		std::cout << "Load xml file err\n";
		return;
	}
	LoadXml(node);
}

CXml::CXml(const XMLNode& xmlNode)
{
	LoadXml(xmlNode);
}

bool CXml::LoadXml(const XMLNode& xmlNode)
{
	XMLNode node = xmlNode;
	const char* nodeName = node.getName();
	if(strcmp(nodeName, "xml") == 0)
	{
		node = node.getChildNode("configuration");
		nodeName = node.getName();
		if(nodeName != NULL && (strcmp(nodeName, "configuration") == 0))
		{
			xmlRoot = node;
			return true;
		}
	}
	std::cout <<"load xml file err\n";
	return false;
}

XMLNode CXml::getXmlNode(const string& strXml)
{
	string xmlstr = strXml;
	size_t pos;
	XMLNode node = xmlRoot;
	string nodeName;
	while(xmlstr.length())
	{
		pos = xmlstr.find('/');
		if(pos == string::npos)
		{
			nodeName = xmlstr;
			xmlstr = "";
		}
		else
		{
			nodeName = xmlstr.substr(0, pos);
			xmlstr = xmlstr.substr(pos+1);		
		}
		node = node.getChildNode(nodeName.c_str());
	}
	if(node.isEmpty())
	{
		std::cout << "getXmlNode cannot find the item \n";
		return NULL;
	}
	return node;
}

string CXml::getString(const string& strXml)
{
	string xmlstr = strXml;
	size_t pos;
	XMLNode node = xmlRoot;
	string nodeName;
	while(xmlstr.length())
	{
		pos = xmlstr.find('/');
		if(pos == string::npos)
		{
			nodeName = xmlstr;
			xmlstr = "";
		}
		else
		{
			nodeName = xmlstr.substr(0, pos);
			xmlstr = xmlstr.substr(pos+1);			
		}
		node = node.getChildNode(nodeName.c_str());
	}
	if(node.isEmpty())
	{
		std::cout << "getString cannot find the item \n";
		return NULL;
	}		
	return node.getText();
}

int CXml::getInteger(const string& strXml)
{
	long long  value;
	string nodeText = getString(strXml);
	if(nodeText.empty())
	{
		std::cout << "getInteger cannot find the item \n";
		return -1;
	}
	sscanf(nodeText.c_str(), "%lld", &value);
	return value;
}

bool CXml::getBoolean(const string& strXml)
{
	string value = getString(strXml);
	if(value[0] == 'B' || value[0] == 'b')
		return true;
	else
		return false;
}

bool CXml::updateNode(const string& strXml, const string& value)
{
	XMLNode node = getXmlNode(strXml);
    node.updateText(value.c_str());
	xmlRoot.writeToFile(xmlFile.c_str());
}

int main(int argc, char *argv[])
{
    CXml xml("./testXML.xml");
	string username = xml.getString("dbsource/username");
	string passwd = xml.getString("dbsource/passwd");
	cout << username << " " << passwd << "\n";
	//	xml.updateNode("dbsource/username", "haha");

	XMLNode node = xml.getRoot().getChildNode("tables");
	int nchild = node.nChildNode("table");
	cout << nchild << endl;
	int iterator;
	for(int i=0;i<nchild;++i)
	{
		iterator = i;
		cout << node.getChildNode("table", &iterator).getAttribute("name") << endl;
		iterator = i;
		cout << node.getChildNode("table", &iterator).getAttribute("type") << endl;
	}
    return 0;
}
