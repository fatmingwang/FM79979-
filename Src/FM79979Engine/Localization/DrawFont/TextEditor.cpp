#include "StdAfx.h"
#include "TextEditor.h"
#include "TreeNodeInfo.h"
#pragma warning( disable:4996 )
using namespace ATG;
#define WRITE_GROUP_INOT_XML	l_pXMLWriter->StartElement(l_sName);// l_pXMLWriter->AddAttribute("Level",l_iTotalLevel);
#define WRITE_CHILD_INOT_XML    l_pXMLWriter->AddAttribute("Name",l_sName);
namespace DrawFont {

	int		HowMuchEndShouldHas(GCFORM::TreeNode^e_pNode)
	{
		GCFORM::TreeNode^l_pNodeParent = e_pNode->Parent;
		GCFORM::TreeNode^l_pNodeCurrent = e_pNode;
		int	l_i = 0;
		while(l_pNodeParent)
		{//search all father  and self,is self last one
			if(  l_pNodeParent->Nodes[l_pNodeParent->Nodes->Count-1]->Equals(l_pNodeCurrent) )
			{
				l_pNodeCurrent = l_pNodeParent;
				l_pNodeParent = l_pNodeParent->Parent;
				l_i++;
			}
			else
				return l_i;
		}
		return l_i;
	}

	bool	IsThisIsALevel(GCFORM::TreeNode^e_pNode)
	{
		bool	l_bIsThisIsALevel = false;
		//search all parent's children
		GCFORM::TreeNode^l_pNode = e_pNode->Parent->Nodes[0];
		while( l_pNode )
		{	
			if( l_pNode->Nodes->Count )
				l_bIsThisIsALevel = true;
			l_pNode = l_pNode->NextNode;
		}
		return l_bIsThisIsALevel;
	}






	XmlNode^ WriteAllNodes(TreeNode^ tnode,XmlDocument^ d,void*e_pVoid)
	{	
		XmlNode^ n = d->CreateNode(XmlNodeType::Element, tnode->Text, "");
		XMLWriter*     l_pXMLWriter = (XMLWriter*)e_pVoid;

		char	l_sName[MAX_PATH];
		GCSTRING_TO_CHAR(tnode->Text->ToString(),l_sName);
		if( !tnode->Nodes->Count )
		{
			//this one is a attribute
			if( tnode->Parent&&!IsThisIsALevel(tnode) )
			{
				WRITE_CHILD_INOT_XML;
				System::Xml::XmlAttribute^l_pAtt = d->CreateAttribute("Name");
				l_pAtt->Value = tnode->Text;
				n->Attributes->Append(l_pAtt);
			}
			else
				l_pXMLWriter->StartElement(l_sName);
		}
		else
			l_pXMLWriter->StartElement(l_sName);

		if( tnode->Nodes->Count )
		{
			for each (TreeNode^ t in tnode->Nodes)
			{
				if( t->Nodes->Count )
					n->AppendChild(WriteAllNodes(t,d,e_pVoid));
				else
				{
					GCSTRING_TO_CHAR(t->Text->ToString(),l_sName);
					WRITE_CHILD_INOT_XML;
					System::Xml::XmlAttribute^l_pAtt = d->CreateAttribute("Name");
					l_pAtt->Value = t->Text;
					n->Attributes->Append(l_pAtt);			
				}

			}
			l_pXMLWriter->EndElement();
		}
		return n;
	}

	System::Void	TextEditor::AddChildIntoTreeNode()
	{
		 if( !TextEDitor_treeView->SelectedNode )
		 {
			WARNING_MSG("please select node");
			return;
		 }

		 if( (int)MaxLevel_numericUpDown->Value-1<=DNCT::GetSlashCount(this->TextEDitor_treeView->SelectedNode->FullPath) )
		 {
			WARNING_MSG("Over Level");
			return;
		 }
		 System::String^l_pNullString;
		 System::String^l_pString = GetTextStringFrom(l_pNullString);
		 if(!l_pString->Length)
			 return;
		 if( NoSameName_checkBox->Checked )
		 {
			 bool l_b = false;
			 SearchNodes(this->TextEDitor_treeView->Nodes[0],l_pString,&l_b);
			 if(l_b)
				WARNING_MSG("this group is exist");
			 else
				DNCT::TreeNodeAddChild(l_pString,TextEDitor_treeView->SelectedNode);
		 }
		 else
		 if(!DNCT::TreeNodeAddChild(l_pString,TextEDitor_treeView->SelectedNode))
			WARNING_MSG("the node name is exist");
	}

	System::Void	TextEditor::AddGroupIntoTreeNode()
	{
		System::String^l_pNullString;
 		 System::String^l_pString = GetTextStringFrom(l_pNullString);
		 if( l_pString->Length )
		 {
			 l_pString = l_pString->Trim();
			 if(!DNCT::TreeViewAddRoot(l_pString,this->TextEDitor_treeView))						 
				WARNING_MSG("this group is exist");
		 }
	}

	System::Void	TextEditor::DeleteTreeNode()
	{
		 if( WARING_YES_NO("This will delete all nodes child!!") == GCFORM::DialogResult::Yes )
			 this->TextEDitor_treeView->SelectedNode->Remove();		
	}

	System::Void	TextEditor::ReNameTreeNode()
	{
		 if( !this->TextEDitor_treeView->SelectedNode )
			return;
		 System::String^l_pString = GetTextStringFrom(TextEDitor_treeView->SelectedNode->Text);
		 if( l_pString->Length )
		 {
			//not on top
			if( TextEDitor_treeView->SelectedNode->Parent )
			{
				if(!DNCT::IsNodesContainName(l_pString,this->TextEDitor_treeView->SelectedNode->Parent->Nodes))
					TextEDitor_treeView->SelectedNode->Text = l_pString;
				else
					WARNING_MSG("name exist");
			}
			else
			{
				if(!DNCT::IsNodesContainName(l_pString,this->TextEDitor_treeView->Nodes))
					TextEDitor_treeView->SelectedNode->Text = l_pString;
				else
					WARNING_MSG("name exist");
			}
		 }
	}

	System::String^	TextEditor::GetTextStringFrom(String^e_str)
	{
		EDitText^	l_p = gcnew EDitText(e_str);
		l_p->ShowDialog();
			return l_p->m_pString;
	}


	System::Void	TextEditor::SearchNodes(GCFORM::TreeNode^e_pNode,System::String^e_pStringToCompare,bool* e_b )
	{
		if( e_pNode->Text->Equals(e_pStringToCompare) )
			*e_b = true;

		if (e_pNode->NextNode)
		{
			SearchNodes(e_pNode->NextNode,e_pStringToCompare,e_b);
		}

		if (e_pNode->Nodes->Count)
		{
			SearchNodes(e_pNode->Nodes[0],e_pStringToCompare,e_b);
		}
	}


	System::Void	TextEditor::WriteInformation(XMLWriter*e_pWriter)
	{
		__time64_t	l_ExportTime;
		l_ExportTime = _time64( NULL );
		CHAR* strTime = _ctime64( &( l_ExportTime ) );

		CHAR* pLF = strchr( strTime, '\n' );
		if( pLF != NULL )
			*pLF = '\0';
		e_pWriter->StartComment();
		e_pWriter->WriteStringFormat( "Time: %s", strTime );
		e_pWriter->EndComment();

		e_pWriter->StartComment();
		e_pWriter->WriteStringFormat( "Exporter: %s",  "Fatming" );
		e_pWriter->EndComment();

		e_pWriter->StartComment();
		char	l_temp[MAX_PATH];
		sprintf(l_temp,"%d\0",this->TextEDitor_treeView->GetNodeCount(true));
		e_pWriter->WriteStringFormat( "TotalCount: %s",  l_temp );
		e_pWriter->EndComment();
	}




	System::Void	TextEditor::SaveGameFile(System::String^e_pString)
	{
		char	l_temp[MAX_PATH];
		if(e_pString->Length)
		{
			e_pString = DNCT::GetFileNameWithoutFullPath(e_pString,true);
			e_pString+=".fat";
			GCSTRING_TO_CHAR(e_pString,l_temp);
		}
		else
		{
			System::String^l_pString = DNCT::SaveFileAndGetName();	
			if( l_pString->Length )
			{
				GCSTRING_TO_CHAR(l_pString,l_temp);
			}
			else
				return;
		}
		XMLWriter     l_XMLWriter(l_temp);
		WriteInformation(&l_XMLWriter);
		l_XMLWriter.StartElement("GameData");
		if(  this->TextEDitor_treeView->Nodes->Count )
		{
			 XmlDocument^ d = gcnew XmlDocument();
			 XmlNode^ n = d->CreateNode(XmlNodeType::Element,"root","");
			 for each (TreeNode^ t in TextEDitor_treeView->Nodes)
			   n->AppendChild(WriteAllNodes(t, d,(void*)&l_XMLWriter));
		
		}
		l_XMLWriter.EndElement();
		l_XMLWriter.Close();
	}

	System::Void	TextEditor::OpenFile()
	{
		System::String^l_pStringForFileName = DNCT::OpenFileAndGetName("xml files (*.xml)|*.xml|All files (*.*)|*.*");
		if( l_pStringForFileName->Length>0 )
		{
			System::Xml::XmlDataDocument^ xmldoc = gcnew System::Xml::XmlDataDocument();
			System::IO::StreamReader ^sr = gcnew  System::IO::StreamReader(l_pStringForFileName);
			System::String^l_pString = sr->ReadToEnd();
			try 
			{
				// SECTION 1. Create a DOM Document and load the XML data into it.
				xmldoc->LoadXml(l_pString);

				// SECTION 2. Initialize the TreeView control.
				GCFORM::TreeView^l_pTreeView = gcnew GCFORM::TreeView();
				TextEDitor_treeView->Nodes->Clear();
				l_pTreeView->Nodes->Add(gcnew TreeNode(xmldoc->DocumentElement->Name));
				TreeNode^ tNode = l_pTreeView->Nodes[0];
				// SECTION 3. Populate the TreeView with the DOM nodes.
				DNCT::AddAttributesAndChildren(xmldoc->DocumentElement, tNode);
				//DNCT::AddNode(xmldoc->DocumentElement, tNode);
				for(int i=0;i<tNode->Nodes->Count;++i)							
					TextEDitor_treeView->Nodes->Add((GCFORM::TreeNode^)tNode->Nodes[i]->Clone());
			}
			catch(XmlException^ xmlEx)
			{
				WARNING_MSG(xmlEx->Message);
			}
			catch(Exception^ ex)
			{
				WARNING_MSG(ex->Message);
			}
			sr->Close();
		}
	}

	bool	TextEditor::SaveXmlFile(System::Object^  sender)
	{
		 System::String^l_pString;
		 if( SaveToolStripMenuItem == sender )
		 {
			 if( m_pStringForFileName && m_pStringForFileName->Length)
				 l_pString = m_pStringForFileName;
			 else
			 {
				m_pStringForFileName = DNCT::SaveFileAndGetName("xml files (*.xml)|*.xml|All files (*.*)|*.*");
				l_pString = m_pStringForFileName;
			 }
		 }
		 else
		 {
			l_pString = DNCT::SaveFileAndGetName("xml files (*.xml)|*.xml|All files (*.*)|*.*");
			if(!m_pStringForFileName||!m_pStringForFileName->Length)
				m_pStringForFileName = l_pString;
		 }
		 if( !l_pString||!l_pString->Length )
			 return false;
		 XmlDocument^ d = gcnew XmlDocument();
		 XmlNode^ n = d->CreateNode(XmlNodeType::Element,"root","");
		 for each (TreeNode^ t in TextEDitor_treeView->Nodes)
		 {
			 n->AppendChild(DNCT::getXmlNode(t, d));
		 }

		  d->AppendChild(n);
		  d->Save(l_pString);
		  return true;
	}


//end namespace DrawFont
}



