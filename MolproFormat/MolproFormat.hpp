#ifndef MOLPROFORMAT_HPP
#define MOLPROFORMAT_HPP

#include<iostream>
#include<string>
#include<boost/regex.hpp>
#include<map>
#include<cstdlib>
#include<vector>

typedef unsigned int uint;
using std::cout;
using std::cerr;
using std::getline;
using std::istream;
using std::string;
using std::endl;
using std::vector;
using std::map;
using std::make_pair;
using boost::regex_match;
using boost::regex_search;
using boost::regex;



template< typename T >
class MolproFormat{
public:
	MolproFormat(const char * streamName_);
	bool readHead(istream & inp);
	bool numberShell(string str, int& L);
	bool fromTo(string str,  uint& from, uint& to); 
	void readEnd(istream & inp);
	bool getElementName(istream & inp,vector<string> & elementLabel, int & exitCode, int & noLine_, string & str);
	bool getElementContent(istream & inp,vector<string>& elementLabel, vector < vector < vector < pair < T, T> > > > & elementContent, int & exitCode, int & noLine_, string & str);
private:
	int noLine;
	const char * streamName;
	regex numberPars, null, expCoeff, znak, expIndexWGC, expCoeffWGC, firstLine, lastLine, empty,comment, emptyString, commentOnly, NoLShell, LShell,expIndex, pars, element;
};

template< typename T >
MolproFormat<T>::MolproFormat(const char * streamName_){
	noLine=0;
	streamName = streamName_;
	firstLine=regex("^\\s*basis\\s*=\\s*\\{\\s*(!.*)*$");//basis={		// FIXME Таки добиться нормального объявления
	lastLine=regex("^\\s*\\}\\s*(!.*)*$"); // $END
	empty=regex("^\\s*$");//пустая строка
	comment=regex("^\\s*(!.*)*");
	pars=regex("\\s*[^\\s]+");
	null=regex("");
	numberPars=regex("-?\\d+(\\.?\\d+)?([e|E|d|D][-|+]\\d+)?");
	element=regex("^\\s*!\\s*([a-zA-Z]{4,})\\s*.*\\((\\d+[sSpPdDfFgGhHiIkKlLmMnN]\\s*,?)+\\)+.*$");
	expIndex=regex("^\\s*([a-zA-Z])\\s*,\\s*[a-zA-Z]+\\s*,\\s*(-?\\d+\\.?\\d+[e|E|d|D]?[-|+]?\\d{0,}\\s*,?\\s*)+$");
	expIndexWGC=regex("^\\s*([a-zA-Z])\\s*,\\s*[a-zA-Z]+\\s*,\\s*(-?\\d+\\.?\\d+[e|E|d|D]?[-|+]?\\d{0,}\\s*,?\\s*)+\\s*(!.*)*$");
	expCoeff=regex("^\\s*[cC]\\s*,\\s*(-?\\d+)\\.?(-?\\d+)\\s*,\\s*(-?\\d+\\.?[\\d+]?[e|E|d|D]?[-|+]?\\d*\\s*,?\\s*)+$");
	expCoeffWGC=regex("^\\s*[cC]\\s*,\\s*(-?\\d+)\\.?(-?\\d+)\\s*,\\s*\\s*((-?\\d+\\.{0,1}\\d+[e|E|d|D]?[-|+]?\\d{0,})\\s*,?\\s*)+\\s*(!.*)*$");
}

template<typename T>
bool MolproFormat<T>::fromTo(string str, uint& from, uint& to){
	boost::smatch resFromTo;
	if(!regex_search(str,resFromTo,expCoeff)) return false;
	from=atoi((resFromTo[1].str()).c_str());
	to=atoi((resFromTo[2].str()).c_str());
	if(from<=0 || to<=0) return false;
	return true;
}

template <typename T>
bool MolproFormat<T>::numberShell(string str, int& L){
	boost::smatch orbNumb;
	if(!regex_match(str,orbNumb,expIndex)) return false;
	int lTmp=-2;
	if(orbNumb[1].str()=="s" || orbNumb[1].str()=="S") lTmp=0;	
	if(orbNumb[1].str()=="p" || orbNumb[1].str()=="P") lTmp=1;	
	if(orbNumb[1].str()=="d" || orbNumb[1].str()=="D") lTmp=2;	
	if(orbNumb[1].str()=="f" || orbNumb[1].str()=="F") lTmp=3;	
	if(orbNumb[1].str()=="g" || orbNumb[1].str()=="G") lTmp=4;	
	if(orbNumb[1].str()=="h" || orbNumb[1].str()=="H") lTmp=5;	
	if(orbNumb[1].str()=="i" || orbNumb[1].str()=="I") lTmp=6;	
	if(orbNumb[1].str()=="k" || orbNumb[1].str()=="K") lTmp=7;	
	if(orbNumb[1].str()=="l" || orbNumb[1].str()=="L") lTmp=8;
	if(orbNumb[1].str()=="m" || orbNumb[1].str()=="M") lTmp=9;
	if(orbNumb[1].str()=="n" || orbNumb[1].str()=="N") lTmp=10;
	if(lTmp==-2) return false;
	L=lTmp;	
	return true;

}

	
template< typename T >
bool MolproFormat<T>::readHead(istream & inp){
	string str;
	while(getline(inp,str)){
		noLine++;
		if(regex_match(str,empty) || regex_match(str,comment)) continue;  
		if(regex_match(str,firstLine)) return true;
		cerr << "Ошибка!!! В строке " << noLine << " \""<<str<<"\"\n  ошибка, чтение из "<< streamName<<" не будет продолжено! " << endl;
		return false;
	}
	cerr << "В " << streamName << " нет данных! " << endl;
	return false;
}




template <typename T>
bool  MolproFormat<T>::getElementName(istream & inp, vector<string> & elementLabel, int & exitCode, int& noLine_, string & str){
	
	elementLabel.clear();
		if(regex_match(str,element)){
			boost::sregex_iterator it(str.begin(),str.end(),pars);
			boost::sregex_iterator itbad;
			while(it!=itbad){
				elementLabel.push_back((*it++).str());
			}
			return true;	
		}
	while(getline(inp,str)){
		noLine++;
		if(regex_match(str,empty)){
			continue;
		}
		if((regex_match(str,comment)) && !regex_match(str,element)){
			continue;
		}
		if(regex_match(str,element)){
			boost::sregex_iterator it(str.begin(),str.end(),pars);
			boost::sregex_iterator itbad;
			while(it!=itbad){
				elementLabel.push_back((*it++).str());
			}
			return true;	
		}
		if(regex_match(str,lastLine)){
			return false; 
		}
		cerr << "Ошибка!!! Строка " << noLine << " : \"" << str << "\"\n не может быть названием элемента."<<
			" Чтение из "<< streamName<<" не будет продолжено! " << endl;
		exitCode = 1; // Не распознан элемент
		return false;
		}

	cerr << "Ошибка - неожиданный конец ввода "<< streamName <<". Не обнаружена завершающая базисный набор фигурная скобка \"}\" \n";
	exitCode = 2;  // Файл оборван
	return false;
		
}


template <typename T>
bool MolproFormat<T>::getElementContent(istream & inp, vector<string>& elementLabel, vector < vector < vector < pair < T, T> > > > & elementContent, int & exitCode, int & noLine_ , string & str){

		elementContent.clear();
		int L=-2;
		int Lmax=0;
		uint from=0;
		uint to=0;
		int help=0;
		bool findNewShell=false;
		bool noCoeff=true;
		vector <double> index;
		vector <double> cfc;
		vector < pair <T,T> > primitive;
		vector < vector <pair <T,T> > > basisFunc;
		 
		while(getline(inp,str)){
			noLine++;
			if(elementContent.empty()){
				if(regex_match(str,element)){ 
					boost::sregex_iterator it(str.begin(),str.end(),pars);
					boost::sregex_iterator itbad;
					while(it!=itbad)
					elementLabel.push_back((*it++).str());
					continue; 
				}
			}
			if(regex_match(str,empty)){
				continue;
			}
			if(regex_match(str,comment) && !regex_match(str,element)){
				continue;
			}
			if(regex_match(str,element)){
				return true;
			}
			if(regex_match(str,lastLine)){
				exitCode=5;
				return true;
			}
			if(regex_match(str,expCoeff)){
				cerr << "В строке " << noLine << " : \n";
				cerr << str << endl;
				cerr << "Ошибка! После названия элемента должна следовать строка, определяющая оболочку, а строка " << noLine;
				cerr << " содержит коэффициенты перед экспонентами!";
				cerr << "Чтение файла не будет продолжено" << endl;
				return false;
			}
			point:
			//noCoeff=true;
			if(regex_match(str,expIndexWGC))
				str=regex_replace(str,comment,null);  
			if(regex_match(str,expIndex)){//встретили строку с показателями экспонент
				noCoeff=true;
				index.clear();
				cfc.clear();
				primitive.clear();
				basisFunc.clear();
				if(regex_match(str,lastLine)){
					cerr << "В строке " << noLine << " : \n";
					cerr << str << endl;
					cerr << "Неожиданный конец файла! " << endl;
					return false;
				}
				if(!numberShell(str,L)){
					cerr << "Ошибка! В строке " << noLine << " : \n";
					cerr << str << endl;
					cerr << "Не распознана оболочка!" << endl;
					return false;
				}
				if(L>=Lmax){
					Lmax=L;
					elementContent.resize(Lmax+1);
				}
				if(L<Lmax){
					cerr << "В строке " << noLine << " : \n";
					cerr << str << endl;
					cerr << "Ошибка! Перед этой строкой";
					cerr << " ожидалось название элемента!" << endl;
					cerr << "Чтение файла не будет продолжено!" << endl;
					return false;
				}
				boost::sregex_iterator numbIterator(str.begin(),str.end(),numberPars);
				boost::sregex_iterator itbad;
				while(numbIterator != itbad){//засунули показатели в вектор
					index.push_back(atof(((*numbIterator++).str()).c_str()));		
				}
				getline(inp,str);
				noLine++;
				while(1){
					if(regex_match(str,empty)){
						getline(inp,str);
						noLine++;
						continue;
					}
					if(regex_match(str,element)){
						if(noCoeff) break;
						elementContent[L]=basisFunc;
						return true;
					}
					if(regex_match(str,lastLine)){
						if(noCoeff) break;
						elementContent[L]=basisFunc;
						exitCode=5;
						return true;
					}
					if(regex_match(str,comment) && !regex_match(str,element)){
						getline(inp,str);
						noLine++;
						continue;
					}
					if(regex_match(str,expIndex) || regex_match(str,expIndexWGC)){
						if(noCoeff) break;
						elementContent[L]=basisFunc;
						findNewShell=true;
						break;
					}
					if(regex_match(str,expCoeff) || regex_match(str,expCoeffWGC)){
						noCoeff=false;
						if(regex_match(str,expCoeffWGC))
							str=regex_replace(str,comment,null);
						cfc.clear();
						primitive.clear();
						if(!fromTo(str,from,to)){
							cerr << "В строке " << noLine << " : \n";
							cerr << str << endl;
							cerr << "Невозможно распознать расстановку коэффициентов перед экспонентами.";
							cerr << " Чтение файла не будет продолжено!" << endl;
							return false; 
						}
						boost::sregex_iterator coef(str.begin(),str.end(),numberPars);
						boost::sregex_iterator inv;
						while(coef!=inv){
							help++;
							cfc.push_back(atof(((*coef++).str()).c_str()));
							if(help==1)
							cfc.erase(cfc.begin());
						}
						help=0;
						if(cfc.size()!=to-from+1){
							cout << "В строке " << noLine << " : \n";
							cout << str << endl;
							cout << "Указанное число коэффициентов не соответствует их истинному количеству!\n";
							cout << "Чтение файла не будет продолжено!" << endl;
							return false;
						}
						int q=0;
						for(uint i=from-1; i<=to-1; i++){
							primitive.push_back(make_pair(index[i],cfc[q]));
							q++;
						}
						basisFunc.push_back(primitive);
						getline(inp,str);
						noLine++;
						continue;
						//TODO сделать проверку на наличие коэффициентов после показателей.
					}
					cerr << "Невозможно считать строку " << noLine << " : \n";
					cerr << str << endl;
					cerr << "Чтение файла не будет продолжено!" << endl;
					return false;
					getline(inp,str);
					noLine++;
				}
				if(noCoeff){
					cerr << "В строке " << noLine << " : \n";
					cerr << str << endl;
					cerr << "Должны быть коэффициенты! Чтение файла не будет продолжено!" << endl;
					return false;
				}				
				if(findNewShell)
				goto point;
				

			}



	}
	cerr << "В строке " << noLine << " : \n";	
	cerr << str << endl;
	cerr << "Неизвестная ошибка! Чтение файла не будет продолжено!" << endl;
	return false;
}
	

template< typename T >
void MolproFormat<T>::readEnd(istream & inp){
	string str;
	while( getline(inp,str) ){
		noLine++;
		if(regex_match(str,empty) || regex_match(str,comment)) continue;  
		cout << "Предупреждение. Строка " << noLine << " \""<<str<<"\"\n в "<<streamName
		     <<" не пустая и не закомментированная, хотя идет после \"}\". Обратите на это внимание.\n";
	}
}

#endif
