#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include <iterator>
#include"GamessFormat.hpp"

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

template< typename T >
class SingleBasisSet{
private:
	map< vector< string> , vector < vector < vector < pair < T, T> > > > > content;
public:
//	TODO операции с несколькими классами, объединение
	SingleBasisSet();
	~SingleBasisSet();
	void printBasisSet() const ;
	bool importBasisSetMolproFormat( const char * fileName);		// Приоритет 1 (Дима, Ваня)
	bool importBasisSetMolproIntFormat( const char * fileName);		// Приоритет 1 (Ваня, Дима)
	bool importBasisSetGamessFormat( const char * fileName);	
	bool importBasisSetPiterFormat( const char * fileName){ return true;};	// Приоритет 1 (Ваня)
	bool exportBasisSetMolproFormat( const char * fileName) const;  // Приоритет 3 
	bool exportBasisSetGamessFormat( const char * fileName) const;	// Приоритет 2
	void sortByAlphaDecrease(){};			// Приоритет 3
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
		for (int i=0; i<int(it->second.size()); i++){
			cout<<"    Оболочка с l="<<i<<'\n';
			for (int j=0; j<int(it->second[i].size()); j++){
				cout<<"        Базисная функция № "<<j+1<<'\n';
				cout<<"\t\t\talpha\tC\n";
				for (int l=0; l<int(it->second[i][j].size()); l++)
					cout<<"\t\t\t"<<it->second[i][j][l].first<<'\t'<<it->second[i][j][l].second<<'\n';
			}	

		}
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
	content.clear();
	while (gamess.getElementName(inp,elementLabel,exitCode,noLine,str)){
		if ( content.find(elementLabel) != content.end() ){
			cerr << "Ошибка в строке\n  " << noLine << " : " << str << "\n   такой элемент уже встречался в "<<fileName<<endl;
			inp.close();
			return false;
		}
		if (!gamess.getElementContent(inp,elementContent,exitCode,noLine,str)) {
			inp.close();
			return false;
		}
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

template< typename T >
bool SingleBasisSet<T>::exportBasisSetMolproFormat( const char * fileName) const{
	return true;	
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetGamessFormat( const char * fileName) const{
	return true;
}


#endif
