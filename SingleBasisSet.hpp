#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<boost/regex.hpp>

using std::vector;
using std::map;
using std::pair;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::getline;
using namespace boost;


template< typename T >
class SingleBasisSet{
private:
	map< vector< string> , vector < vector < vector < pair < T, T> > > > > content;
//	непонятно, надо имя или нет tring 
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
	cout<<"Число элементов в базисном наборе: " << content.size() <<'\n';
	
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
	regex empty("^\\s*$");
	regex emptyOrComment("^\\s*(!.*)*$");
	regex firstLine("^\\s*\\$DATA\\s*$");
//	Проверка шапки файла
//
//	
	while( getline(inp,str) ){
		noLine++;
		if(regex_search(str,emptyOrComment)) continue;  
		
//	Если строка пустая, то дальше
//	Если строка коммент, то дальше
//	Если строка стартовая, то смена флага и выход из while
//	выход из while
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


//	Заполнение элементов
	while( getline(inp,str) ){
	}

//	Проверка корректного завершения файла
	while( getline(inp,str) ){
	}




	inp.close();

//	Формат файла
//	{пустая строчка или строчка из комментариев}
//	{начальная строчка}
//	блок{
//	строка комментариев или любая строка из слов, которые будут определять элемент
//	блок{
//		Буква, целое [, комментарий] 
//		целое, double, double, [double если буква = L] [!, комментарий]
//	}
//	пустая строка или конечная строчка
//	}
//	пустая строка *
//	конечная строка*
//	{пустая строчка или строка комментариев}

	elementLabel.push_back("CARBON");
	content[elementLabel]=elementContent;
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

