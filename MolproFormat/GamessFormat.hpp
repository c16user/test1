#ifndef GAMESSFORMAT_HPP
#define GAMESSFORMAT_HPP

#include<iostream>
#include<string>
#include<boost/regex.hpp>
#include<map>
#include <cstdlib>
#include<vector>

using std::cout;
using std::cerr;
using std::getline;
using std::istream;
using std::string;
using std::endl;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using boost::regex_match;
using boost::regex_search;
using boost::regex;


template< typename T >
class GamessFormat{
public:
	GamessFormat(const char * streamName_);
	bool readHead(istream & inp);
	void readEnd(istream & inp);
	bool getElementName(istream & inp,vector<string> & elementLabel, int & exitCode, int & noLine_, string & str);
	bool getElementContent(istream & inp, vector < vector < vector < pair < T, T> > > > & elementContent,
	                int & exitCode, int & noLine_, string & str);
private:
	int noLine;
	const char * streamName;
	regex firstLine, lastLine, emptyOrComment, emptyString, commentOnly, NoLShell, LShell,shell, pars, el;
	bool getLandNoPrimitives(string str, int & L, int & N) const;
};

template< typename T >
GamessFormat<T>::GamessFormat(const char * streamName_){
	noLine=0;
	streamName = streamName_;
	firstLine=regex("^\\s*\\$DATA\\s*(!.*)*$");//$DATA		// FIXME Таки добиться нормального объявления
	lastLine=regex("^\\s*\\$END\\s*(!.*)*$"); // $END
	emptyOrComment=regex("^\\s*(!.*)*$");//пустая строка или коммент.
	shell=regex("^\\s*([a-zA-Z])\\s+(\\d+)\\s*$");//оболочка
	emptyString=regex("^\\s*$");
	el=regex("^\\s*[a-zA-Z]{4,}.*$");
	pars=regex("\\s*[^\\s]+");
	commentOnly=regex("^\\s*!.*$");
	NoLShell=regex("^\\s*\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*(!.*)*$");//не L-оболочка
	LShell=regex("^\\s*\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s*(!.*)*$");
}
	
template< typename T >
bool GamessFormat<T>::readHead(istream & inp){
	string str;
	while( getline(inp,str) ){
		noLine++;
		if(regex_match(str,emptyOrComment)) continue;  
		if(regex_match(str,firstLine)) return true;
		cerr << "Ошибка!!! В строке " << noLine << " \""<<str<<"\"\n  ошибка, чтение из "<< streamName<<" не будет продолжено! " << endl;
		return false;
	}
	cerr << "В " << streamName << " нет данных! " << endl;
	return false;
}

template< typename T >
void GamessFormat<T>::readEnd(istream & inp){
	string str;
	while( getline(inp,str) ){
		noLine++;
		if(regex_match(str,emptyOrComment)) continue;  
		cout << "Предупреждение. Строка " << noLine << " \""<<str<<"\"\n в "<<streamName
		     <<" не пустая и не закомментированная, хотя идет после $END. Обратите на это внимание.\n";
	}
}

template< typename T >
bool GamessFormat<T>::getLandNoPrimitives(string str, int & L, int & N) const{
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
bool  GamessFormat<T>::getElementContent(istream & inp, vector < vector < vector < pair < T, T> > > > & elementContent,
	                int & exitCode, int & noLine_, string & str){
	double Ai,Ci,CLi;
	int N, L;
	boost::smatch result;
	boost::smatch CiAiNoL;
	boost::smatch CiAiL;
	elementContent.clear();
	elementContent.resize(20 ); // с запасом, потом отрезать
	int Lmax=-1; //	а это как раз будет запоминать максимальную L, чтобы остальное отрезать
	vector < pair < T, T > > basisFunc1;
	vector < pair < T, T > > basisFunc2;  // для L оболочки
	while (getline(inp,str) ) {
		noLine++;
		if(regex_match(str,emptyString)){
			elementContent.resize(Lmax+1);
			return true;
		}
		if(regex_match(str,commentOnly)) continue; 
		if(regex_match(str,lastLine)){
			exitCode = 5; 	// FIXME Magic constant - это очень плохо!
			elementContent.resize(Lmax+1);
			return true;
		}
		if (!regex_match(str,result,shell)){
			cerr << "Ошибка!!! Строка " << noLine << " : \"" << str << "\"\n должна быть описанием оболочки.\n";
			return false;
		}
		if(!getLandNoPrimitives(str,L,N)){
			cerr << "Ошибка!!! В строке " << noLine << " : \"" << str << "\"\n   ожидалось чтение заголовка в формате \"оболочка - число примитивов\"\n";
			return false;
		}
		// вот тут мы можем находиться после того, как считалась оболочка
		if (Lmax<abs(L)) Lmax=abs(L);
		basisFunc1.clear();
		basisFunc2.clear();
		for (int i=0; i<N; i++){
			if(!getline(inp,str)){
				cerr << "Ошибка - неожиданный конец ввода "<< streamName <<". Не обнаружено ключевое слово $END\n";
				return false;
			}
			noLine++;
			if(regex_match(str,commentOnly)){
				i--;
				continue; 
			}
			if ((L==-1)&&(regex_match(str,CiAiL,LShell))){
				Ai=atof((CiAiL[1].str()).c_str());
				Ci=atof((CiAiL[2].str()).c_str());
				CLi=atof((CiAiL[3].str()).c_str());
				cout << Ai << " " << Ci << " " << CLi << endl;
				basisFunc1.push_back(make_pair(Ai,Ci));
				basisFunc2.push_back(make_pair(Ai,CLi));
				continue;
			}
			if ((L>-1)&&(regex_match(str,CiAiNoL,NoLShell))){
				Ai=atof((CiAiNoL[1].str()).c_str());
				Ci=atof((CiAiNoL[2].str()).c_str());
				cout << Ai << " " << Ci << endl;
				basisFunc1.push_back(make_pair(Ai,Ci));
				continue;
			}
			cerr << "Ошибка!!! Строка " << noLine << " : \"" << str << "\"\n не может описывать примитив!"<<
			" Чтение из "<< streamName<<" не будет продолжено! " << endl;
			return false;
		}
		if(L == -1){
			elementContent[0].push_back(basisFunc1);
			elementContent[1].push_back(basisFunc2);
		} else {
			elementContent[L].push_back(basisFunc1);
		}
	}
	cerr << "Ошибка - неожиданный конец ввода "<< streamName <<". Не обнаружено ключевое слово $END\n";
	return false;
}

template< typename T >
bool  GamessFormat<T>::getElementName(istream & inp, vector<string> & elementLabel, int & exitCode, int & noLine_, string & str){
	
	while( getline(inp,str) ){
		noLine++;
		if ( regex_search(str,emptyOrComment)) continue;
		if ( regex_match(str,lastLine ) ) return false;
		if ( regex_match ( str,el)){
			elementLabel.clear();
			boost::sregex_iterator it(str.begin(),str.end(),pars);
			boost::sregex_iterator itbad;//этот if-блок-парсинг строки с названием элемента.
			while (it != itbad) elementLabel.push_back((*it++).str());
			return true;
		}
		cerr << "Ошибка!!! Строка " << noLine << " : \"" << str << "\"\n не может быть названием элемента."<<
			" Чтение из "<< streamName<<" не будет продолжено! " << endl;
		exitCode = 1; // Не распознан элемент
		return false;
	}
	cerr << "Ошибка - неожиданный конец ввода "<< streamName <<". Не обнаружено ключевое слово $END\n";
	exitCode = 2;  // Файл оборван
	return false;
}
		

#endif
