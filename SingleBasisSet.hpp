#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<boost/regex.hpp>
#include"GamessFormat.hpp"
//#include <utility>

using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::getline;
using boost::regex_replace;
using boost::regex_match;
using boost::regex_search;
using boost::regex;

template< typename T >
class SingleBasisSet{
private:
	map< vector< string> , vector < vector < vector < pair < T, T> > > > > content;
public:
//	TODO операции с несколькими классами, объединение
	SingleBasisSet();
	~SingleBasisSet();
	void printBasisSet() const ;
	bool importBasisSetMolproFormat( const char * fileName);
	bool importBasisSetGamessFormat( const char * fileName);
	bool exportBasisSetMolproFormat( const char * fileName) const;
	bool exportBasisSetGamessFormat( const char * fileName) const;
};


template< typename T >
SingleBasisSet<T>::SingleBasisSet(){
	content.clear();
}

template< typename T >
SingleBasisSet<T>::~SingleBasisSet(){}

template< typename T >
void SingleBasisSet<T>::printBasisSet() const{
	cout<<"Число элементов в базисном наборе = " <<content.size()<<'\n';
	for (typename map< vector< string> , vector < vector < vector < pair < T, T> > > > >::const_iterator it = content.begin(); it != content.end(); it++){ // FIXME хрень какая-то! разобраться
		cout<<"Элемент :";
		for ( int i=0; i<int(it->first.size()); i++)
			cout<<' '<<it->first[i];
		cout<<'\n'; 
	}
}

template< typename T >
bool SingleBasisSet<T>::importBasisSetMolproFormat( const char * fileName){
	if ( !content.empty() ) {
		cerr << "Базисный набор уже задан, ошибка!\n";
		return false;
	}
	vector<string> elementLabel;
	vector < vector < vector < pair < T, T> > > > elementContent;
	elementLabel.push_back("carbon");
	content[elementLabel]=elementContent;
	
	return true;
}


template< typename T >
bool SingleBasisSet<T>::importBasisSetGamessFormat( const char * fileName){
//	Проверки
	if ( !content.empty() ) {
		cerr << "Базисный набор уже задан, ошибка!\n";
		return false;
	}
//	Предварительные объявления
	vector<string> elementLabel;
	vector < vector < vector < pair < T, T> > > > elementContent;
	GamessFormat<T> gamess(fileName);
	string str;
	int noLine;
//	Открыть файл
	ifstream inp (fileName); // TODO сделать проверку на существование
//	Прочитать начало файла
	if (!gamess.readHead(inp) ){
			inp.close();
			return false;
	}
////	Читать поэлементно до того места, пока не конец
	int exitCode = 0;
	while (gamess.getElementName(inp,elementLabel,exitCode,noLine,str)){
		if ( content.find(elementLabel) != content.end() ){
			cerr << "Ошибка в строке\n  " << noLine << " : " << str << "\n   такой элемент уже встречался в "<<fileName<<endl;
			return false;
		}
		if (!gamess.getElementContent(inp,elementContent,exitCode,noLine,str)) return false;
		content[elementLabel] = elementContent;
		if (exitCode==5){ // небольшой костыль, есть встретился конец файла
			exitCode=0;
			break;
		}
	}
	if (exitCode !=0) return false;
//	Прочитать конец файла
	gamess.readEnd(inp);
	inp.close();
	return true;
}
/*
	cout.precision(12);
	cout.setf(std::ios::fixed,std::ios::floatfield);



	GamessFormat<T>().printLine(inp);
	return true;

	

	vector < pair < T, T > > basisFunc1;
	vector < pair < T, T > > basisFunc2;  // для L оболочки
	vector < vector < pair < T, T > > > emptyShellVector;
	emptyShellVector.clear();


	regex firstLine("^\\s*\\$DATA\\s*(!.*)*$");//$DATA
	regex lastLine("^\\s*\\$END\\s*(!.*)*$"); // $END

	regex emptyOrComment("^\\s*(!.*)*$");//пустая строка или коммент.
	regex emptyString("^\\s*$");
	regex commentOnly("^\\s*!.*$");

	regex shell("^\\s*([a-zA-Z])\\s+(\\d+)$");//оболочка
	regex pars("\\s*[^\\s]+");
	regex el("^\\s*[a-zA-Z]{4,}.*$");
	regex trash("!.*$");
	string clear("");
	regex NoLShell("^\\s+\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*$");//не L-оболочка
	regex LShell("^\\s+\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*$");

//	Заполнение элементов
	boost::smatch result;
	boost::smatch CiAiNoL;
	boost::smatch CiAiL;
	int L=-3;
	int N;
	double Ci, Ai, mix;
	enum ExitCode { none, badShell, findEnd, badElement, sameElement, endOfShells, badPrimitives, endOfFile } ;
	ExitCode exitCode;

//	Начать считывать элементы
	while(getline(inp,str)) {
		exitCode = none;
		noLine++;
		if ( regex_match(str,lastLine ) ){
			exitCode = findEnd;
			break;	
		}
		if ( regex_match ( str,emptyOrComment ) ) continue;
		str=regex_replace(str,trash,clear); // отброс комментариев в строке  // FIXME может быть сделать strGood или strNotModified
		if ( regex_match ( str,el)){
			elementLabel.clear();
			elementContent.clear();
			elementContent.resize(10 );
			boost::sregex_iterator it(str.begin(),str.end(),pars);
			boost::sregex_iterator itbad;//этот if-блок-парсинг строки с названием элемента.
			while (it != itbad){
				elementLabel.push_back((*it++).str());
			}
			if ( content.find(elementLabel) != content.end() ){
				exitCode = sameElement;
				break;
			}
			// начали читать оболочки
			exitCode = endOfFile;
			while (getline(inp,str) ) {
				noLine++;
				// если пустая строка - то закончить чтение элемента
				if(regex_match(str,emptyString)){
				}
					exitCode = endOfShells;
					break;
				// если комментарий - то continue
				if(regex_match(str,commentOnly)) continue; 
				if(regex_match(str,lastLine)){
					exitCode = findEnd;
					break;
				}
				// если оболочка - то запустить считывание примитивов, иначе эта строка не распознана - break, сообщения, флаги
				if (regex_match(str,result,shell)){
					if(!getLandNoPrimitives(str,L,N)){
						exitCode = badShell;
						break;
					}
					// начало считывания примитивов
					basisFunc1.clear();
					basisFunc2.clear();
					for (int i=0; i<N; i++){
						if(!getline(inp,str)){
							exitCode = endOfFile;
							break;
						}
						noLine++;
						if(regex_match(str,commentOnly)){
							i--; continue; 
						}
						str=regex_replace(str,trash,clear); // отброс комментариев в строке  // FIXME может быть сделать strGood или strNotModified
						if ((L==-1)&&(regex_match(str,CiAiL,LShell))){
							Ai=atof((CiAiL[1].str()).c_str());
							Ci=atof((CiAiL[2].str()).c_str());
							mix=atof((CiAiL[3].str()).c_str());
							cout << Ai << " " << Ci << " " << mix << endl;
							basisFunc1.push_back(make_pair(Ai,Ci));
							basisFunc2.push_back(make_pair(Ai,mix));
							continue;
						}
						if ((L>-1)&&(regex_match(str,CiAiNoL,NoLShell))){
							Ai=atof((CiAiNoL[1].str()).c_str());
        						Ci=atof((CiAiNoL[2].str()).c_str());
							cout << Ai << " " << Ci << endl;
							basisFunc1.push_back(make_pair(Ai,Ci));
							continue;
						}
						exitCode = badPrimitives;
						break;
					}
					// конец считывания примитивов
					if ( int( elementContent.size() ) < int ( fabs ( L ) ) ){
						elementContent.resize( int ( fabs (L) ) +1 );
					}
					if (L>-1) {
						elementContent[L].push_back(basisFunc1);
					} else {
						elementContent[0].push_back(basisFunc1);
						elementContent[1].push_back(basisFunc2);
					}
					continue;
				}
				exitCode = badShell;
				break;
			}
			
			// Тут мы оказываемся со следующими кодами:
			// а) endOfFile   б) badShell    в) endOfShells    г) findEnd   д) badPrimitives
			if (( exitCode == badShell) || ( exitCode == badPrimitives) || (exitCode == endOfFile)) break;
			if (( exitCode == endOfShells )) {
				content[elementLabel] = elementContent;
				continue;
			}
			if (( exitCode == findEnd )) {
				content[elementLabel] = elementContent;
				break;
			}
			std::cout << " ВЫ НЕ ДОЛЖНЫ УВИДЕТЬ ЭТУ ЗАПИСЬ, КОД ВЫХОДА = "<<exitCode<<'\n';
		} else {
			exitCode = badElement;
			break;
		}
	}
	std::cout<<">>>>"<<exitCode<<"<<<\n";
	
	switch (exitCode){
	case (endOfFile):
		cerr << "Ошибка - неожиданный конец файла, файл закончен некорректно\n";
		return false;
	case (badElement):
		cerr << "Ошибка в строке\n  " << noLine << " : " << str << "\n   такая строка не может быть названием элемента\n";
		return false;
	case (sameElement):
		cerr << "Ошибка в строке\n  " << noLine << " : " << str << "\n   такой элемент уже встречался в этом файле\n";
		return false;
	case (badShell):
		cerr << "Ошибка в строке\n  " << noLine << " : " << str << "\n   ожидалось чтение заголовка в формате \"оболочка - число примитивов\"\n";
		return false;
	break;

	}


//	Проверка корректного завершения файла
	int endT=0;
	while( getline(inp,str) ){
		if(regex_search(str,lastLine)) break;
		endT++;//строго, даже строки с символами-разделятелями нельзя
		
	}
	if(endT!=0){
	cout << "После базисного набора и идентификатора $END окончания файла присутствуют непустые строки. ";
	cout << "Если Вам кажется, что строки ниже пустые, проверьте, нет ли в строках, идущих ниже строки \"$END\" символов-разделителей. ";
	}



	inp.close();
	return true;
}*/

template< typename T >
bool SingleBasisSet<T>::exportBasisSetMolproFormat( const char * fileName) const{
	return true;	
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetGamessFormat( const char * fileName) const{
	return true;
}


#endif
