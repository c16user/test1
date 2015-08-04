#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<boost/regex.hpp>
#include <utility>

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
	bool getLandNoPrimitives(string str, int & L, int & N) const;
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
bool SingleBasisSet<T>::getLandNoPrimitives(string str, int & L, int & N) const{
	regex shell("^\\s*([a-zA-Z])\\s+(\\d+)$");
	boost::smatch result;
	if(!regex_search(str,result,shell)) return false;
	int lTmp=-2;
	if ((result[1].str())=="s" || (result[1].str())=="S" ) lTmp=0;
	if ((result[1].str())=="p" || (result[1].str())=="P" ) lTmp=1;
	if ((result[1].str())=="d" || (result[1].str())=="D" ) lTmp=2;
	if ((result[1].str())=="f" || (result[1].str())=="F" ) lTmp=3;
	if ((result[1].str())=="g" || (result[1].str())=="G" ) lTmp=4;
	if ((result[1].str())=="h" || (result[1].str())=="H" ) lTmp=5;
	if ((result[1].str())=="i" || (result[1].str())=="I" ) lTmp=6;
	if ((result[1].str())=="k" || (result[1].str())=="K" ) lTmp=7;
	if ((result[1].str())=="l") lTmp=8;
	if ((result[1].str())=="L") lTmp=-1;
	if ((result[1].str())=="m" || (result[1].str())=="M" ) lTmp=9;
	if ((result[1].str())=="n" || (result[1].str())=="N" ) lTmp=10;
	if (lTmp==-2) return false;
	N=atoi((result[2].str()).c_str());
	if (N==0) cout<<"Предупреждение: задано 0 примитивов.\n";
	L=lTmp;
	return true;
}

template< typename T >
SingleBasisSet<T>::SingleBasisSet(){
	content.clear();
}

template< typename T >
SingleBasisSet<T>::~SingleBasisSet(){}

template< typename T >
void SingleBasisSet<T>::printBasisSet() const{
	cout<<"Число элементов в базисном наборе =0" <<'\n';
	
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

	cout.precision(12);
	cout.setf(std::ios::fixed,std::ios::floatfield);

	if ( !content.empty() ) {
		cerr << "Базисный набор уже задан, ошибка!\n";
		return false;
	}
	vector<string> elementLabel;
	vector < vector < vector < pair < T, T> > > > elementContent;
	ifstream inp (fileName);
	string str;
	bool isEndGood = false;
	bool isBeginGood = false;
	bool isBadLine = false;
	int noLine = 0;
	pair<double,double> AiCi;
	vector<pair<double,double> > primitive;
	vector<vector<pair<double,double> > > shellvector;
	regex emptyOrComment("^\\s*(!.*)*$");//пустая строка или коммент. (все ли правильно в этом рв?)
	regex firstLine("^\\s*\\$DATA\\s*(!.*)*$");//$DATA
	regex shell("^\\s*([a-zA-Z])\\s+(\\d+)$");//оболочка
	regex end("^\\$END");
	regex emptyString("^\\s*$");
	regex pars("\\s*[^\\s]+");
	regex el("^\\s*[a-zA-Z]{4,}.*$");
	regex trash("!.*$");
	regex commentOnly("^!.*$");
	string clear("");
	regex NoLShell("^\\s+\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*$");//не L-оболочка
	regex LShell("^\\s+\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*$");
	regex EndOfFile("^\\s*\\$END\\s*(!.*)*$");

//	Начало проверки шапки файла
	while( getline(inp,str) ){
		noLine++;
		if(regex_search(str,emptyOrComment)) continue;  
		if(regex_search(str,firstLine)) {
			isBeginGood = true;
			break;
		}
		isBadLine = true;
		break;
	}
	if(isBadLine){
		cout << "В строке " << noLine << " \""<<str<<"\"\n"<<" ошибка, чтение файла " << fileName << " не будет продолжено! " << endl;
		return false;
	}
	if(!isBeginGood){
		cout << "В файле " << fileName << " нет данных! " << endl;
		return false;
	}
//	Конец проверки шапки файла
//	Заполнение элементов
	boost::smatch result;
	boost::smatch CiAiNoL;
	boost::smatch CiAiL;
	int L=-3;
	int N;
	double Ci, Ai, mix;
	enum ExitCode { none, badShell, lastLine, badElement, sameElement, endOfShells, badPrimitives } ;
	ExitCode exitCode;

//	Начать считывать элементы
	while(getline(inp,str)) {
		exitCode = none;
		noLine++;
		if ( regex_match(str,EndOfFile ) ){	// FIXME имя переделать :)
			exitCode = lastLine;
			break;	
		}
		if ( regex_match ( str,emptyOrComment ) ) continue;
		str=regex_replace(str,trash,clear); // отброс комментариев в строке  // FIXME может быть сделать strGood или strNotModified
		if ( regex_match ( str,el)){
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
			while (getline(inp,str) ) {
				noLine++;
				// если пустая строка - то закончить чтение элемента
				if(regex_match(str,emptyString)){
					exitCode = endOfShells;
					break;
				}
				// если комментарий - то continue
				if(regex_match(str,commentOnly)) continue; 
				// если оболочка - то запустить считывание примитивов, иначе эта строка не распознана - break, сообщения, флаги
				if (regex_match(str,result,shell)){
					if(!getLandNoPrimitives(str,L,N)){
						exitCode = badShell;
						break;
					}
					// начало считывания примитивов
					for (int i=0; i<N; i++){
						getline(inp,str);
						noLine++;
						// Строка с комментарием - проигнорить, но i--
						if(regex_match(str,commentOnly)){
							i--;
							continue; 
						}
						str=regex_replace(str,trash,clear); // отброс комментариев в строке  // FIXME может быть сделать strGood или strNotModified
						if ((L==-1)&&(regex_match(str,CiAiL,LShell))){
							Ai=atof((CiAiL[1].str()).c_str());
							Ci=atof((CiAiL[2].str()).c_str());
							mix=atof((CiAiL[3].str()).c_str());
							cout << Ai << " " << Ci << " " << mix << endl;
							continue;
						}
						if ((L>-1)&&(regex_match(str,CiAiNoL,NoLShell))){
							Ai=atof((CiAiNoL[1].str()).c_str());
        						Ci=atof((CiAiNoL[2].str()).c_str());
							cout << Ai << " " << Ci << endl;
							AiCi=make_pair(Ai,Ci);//создаются пары показатель-коэфф
							primitive.push_back(AiCi);//пары закладываются в первый вектор
							continue;
						}
						exitCode = badPrimitives;
						break;
					}
					// конец считывания примитивов
					if (exitCode == badPrimitives ) break;
					continue;
				}
				exitCode = badShell;
				break;
			}
			if (( exitCode != none) && ( exitCode != endOfShells)) break;
		} else {
			exitCode = badElement;
			break;
		}
	}
	std::cout<<">>>>"<<exitCode<<"<<<\n";
	
	switch (exitCode){
	case (lastLine):
	break;
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
		if(regex_search(str,end)) break;
		endT++;//строго, даже строки с символами-разделятелями нельзя
		
	}
	if(endT!=0){
	cout << "После базисного набора и идентификатора $END окончания файла присутствуют непустые строки. ";
	cout << "Если Вам кажется, что строки ниже пустые, проверьте, нет ли в строках, идущих ниже строки \"$END\" символов-разделителей. ";
	}



	inp.close();
//	elementLabel.push_back("CARBON");
//	content[elementLabel]=elementContent;
	return true;
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetMolproFormat( const char * fileName) const{
	return true;	
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetGamessFormat( const char * fileName) const{
	return true;
}


#endif
/*
regex S("^S\\s{3}\\d{1,2}");
regex P("^P\\s{3}\\d{1,2}");
regex D("^D\\s{3}\\d{1,2}");
regex F("^F\\s{3}\\d{1,2}");
regex G("^G\\s{3}\\d{1,2}");
regex L("^L\\s{3}\\d{1,2}");
int orbital (string str){
	if(regex_search(str,S)) return 0;
	if(regex_search(str,P)) return 1;
	if(regex_search(str,D)) return 2;
	if(regex_search(str,F)) return 3;
	if(regex_search(str,G)) return 4;
	if(regex_search(str,L)) return 5;
	return 1000;
}*/


/*
regex shell("^[A-Z]\\s{1,3}\\d{1,2}");//оболочка
bool shellsearch (string str){
	return regex_search(str,shell);
}*/

/* Пример разбора строки
	getline(inp,str);

	const boost::sregex_iterator itEmpty;
	boost::sregex_iterator it(str.begin(),str.end(),regex("\\s*[^\\s]+"));
	elementLabel.clear();
	while (it != itEmpty ) elementLabel.push_back((*it++).str());
	content[elementLabel]=elementContent;
	return true;
*/





