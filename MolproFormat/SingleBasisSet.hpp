#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<iterator>
#include<stdio.h>
#include"GamessFormat.hpp"
#include"MolproFormat.hpp"

typedef unsigned unsgd;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
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
	//char shellName(int u);
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
	//проверки
	if ( !content.empty() ) {
		cerr << "Базисный набор уже задан, ошибка!\n";
		return false;
	}
	vector<string> elementLabel;
	vector < vector < vector< pair <T,T > > > > elementContent;
	string str;
	MolproFormat<T> molpro(fileName);
	int noLine;
	int exitCode=0;
	string lastString;
	//открываем файл
	ifstream inp (fileName); 
	if (!molpro.readHead(inp)){
		inp.close();
		return false;
	}
	while (molpro.getElementName(inp,elementLabel, exitCode, noLine,str)){
		if ( content.find(elementLabel) != content.end() ){
			cerr << "Ошибка в строке\n  " << noLine << " : " << str << "\n   такой элемент уже встречался в "<<fileName<<endl;
			inp.close();
			return false;
		}
		if (!molpro.getElementContent(inp,elementLabel,elementContent,exitCode,noLine,str)){
			inp.close();
			return false;
		}
		content[elementLabel] = elementContent;
		if (exitCode==5){ // небольшой костыль, есть встретился конец файла
			exitCode=0;
			break;
		}
	}
	if(exitCode!=0) return false;
	molpro.readEnd(inp);
	inp.close();

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

char shellName(int u){
	if(u==0) return 's';
	if(u==1) return 'p';
	if(u==2) return 'd';
	if(u==3) return 'f';
	if(u==4) return 'g';
	if(u==5) return 'h';
	if(u==6) return 'i';
	if(u==7) return 'k';
	if(u==8) return 'l';
	if(u==9) return 'm';
	if(u==10) return 'n';
	cerr<<"Не распознана оболочка!\n"; 
	return '1';
}

string elementSymbol(string str){
	if(str=="HYDROGEN") return "H";
	if(str=="HELIUM") return "HE";
	if(str=="LITHIUM") return "LI";
	if(str=="BERYLLIUM" || str=="GLUCINIUM") return "BE";
	if(str=="BORON") return "B";
	if(str=="CARBON") return "C";
	if(str=="NITROGEN") return "N";
	if(str=="OXYGEN") return "O";
	if(str=="FLUORINE") return "F";
	if(str=="NEON") return "NE";
	if(str=="NATRIUM" || str=="SODIUM") return "NA";
	if(str=="MAGNESIUM") return "MG";
	if(str=="ALUMINIUM") return "AL";
	if(str=="SILICON") return "SI";
	if(str=="PHOSPHOROUS") return "P";
	if(str=="SULFUR") return "S";
	if(str=="CHLORINE") return "CL";
	if(str=="ARGON") return "AR";
	if(str=="POTASSIUM") return "K";
	if(str=="CALCIUM") return "CA";
	if(str=="SCANDIUM") return "SC";
	if(str=="TITANIUM") return "TI";
	if(str=="VANADIUM") return "V";
	if(str=="CHROMIUM") return "CR";
	if(str=="MANGANESE") return "MN";
	if(str=="IRON") return "FE";
	if(str=="COBALT") return "CO";
	if(str=="NICKEL") return "NI";
	if(str=="COPPER") return "CU";
	if(str=="ZINC") return "ZN";
	if(str=="GALLIUM") return "GA";
	if(str=="GERMANIUM") return "GE";
	if(str=="ARSENIC") return "AS";
	if(str=="SELENIUM") return "SE";
	if(str=="BROMINE") return "BR";
	if(str=="KRYPTON") return "KR";
	if(str=="RUBIDIUM") return "RB";
	if(str=="STRONTIUM") return "SR";
	if(str=="YTTRIUM") return "Y";
	if(str=="NIOBIUM") return "NB";
	if(str=="MOLYBDENUM") return "MO";
	if(str=="TECHNETIUM") return "H";
	if(str=="RUTHENIUM") return "RU";
	if(str=="RHODIUM") return "RH";
	if(str=="PALLADIUM") return "PD";
	if(str=="ARGENTUM") return "AG";
	if(str=="CADMIUM") return "CD";
	if(str=="INDIUM") return "IN";
	if(str=="TIN") return "SN";
	if(str=="STIBIUM" || str=="ANTIMONY") return "SB";
	if(str=="TELLURIUM") return "TE";
	if(str=="IODINE") return "I";
	if(str=="XENON") return "XE";
	if(str=="CAESIUM") return "CS";
	if(str=="BARIUM") return "BA";
	if(str=="LANTHANUM") return "LA";
	if(str=="CERIUM") return "CE";
	if(str=="PRASEODYMIUM") return "PR";
	if(str=="NEODYMIUM") return "ND";
	if(str=="PROMETHIUM") return "PM";
	if(str=="SAMARIUM") return "SM";
	if(str=="EUROPIUM") return "EU";
	if(str=="GADOLINIUM") return "GD";
	if(str=="TERBIUM") return "TB";
	if(str=="DYSPROSIUM") return "DY";
	if(str=="HOLMIUM") return "HO";
	if(str=="ERBIUM") return "ER";
	if(str=="THULIUM") return "TM";
	if(str=="YTTERBIUM") return "YB";
	if(str=="LUTECIUM") return "LU";
	if(str=="HAFNIUM") return "HF";
	if(str=="TANTALUM") return "TA";
	if(str=="TUNGSTEN") return "W";
	if(str=="RHENIUM") return "RE";
	if(str=="OSMIUM") return "OS";
	if(str=="IRIDIUM") return "IR";
	if(str=="PLATINUM") return "PT";
	if(str=="GOLD") return "AU";
	if(str=="MERCURY" || str=="QUICKSILVER") return "HG";
	if(str=="THALLIUM") return "TL";
	if(str=="LEAD") return "PB";
	if(str=="BISMUTH") return "BI";
	if(str=="POLONIUM") return "PO";
	if(str=="ASTATINE") return "AT";
	if(str=="RADON") return "RN";
	if(str=="FRANCIUM") return "FR";
	if(str=="RADIUM") return "RA";
	if(str=="ACTINIUM") return "AC";
	if(str=="THORIUM") return "TH";
	if(str=="PROTACTINIUM") return "PA";
	if(str=="URANIUM") return "U";
	if(str=="PLUTONIUM") return "NP";
	if(str=="AMERICIUM") return "PU";
	if(str=="CURIUM") return "CM";
	if(str=="BERKELIUM") return "BK";
	if(str=="CALIFORNIUM") return "CF";
	if(str=="EINSTEINIUM") return "ES";
	if(str=="FERMIUM") return "FM";
	if(str=="MENDELEVIUM") return "MD";
	if(str=="NOBELIUM") return "NO";
	if(str=="LAWRENCIUM") return "LR";
	if(str=="RUTHERFORDIUM") return "RF";
	return "EN";

}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetMolproFormat( const char * fileName) const{
		ofstream out("outputFile");
		vector <double> search;
		vector <double> uniqueIndex;
		vector <double> coef;
		unsgd from=0;
		unsgd to=0;
		string str;
		string catchName;
		regex a("\\s*,\\s*$");
		regex b("");
		regex elName("[a-zA-Z]{4,}");
		bool alreadyExist=false;
		bool findFrom=false;
		bool findTo=false;
		bool uniq=true;
		out<<"basis=\{"<<endl;
//TODO надо бы сделать вот что: писать не EN в строках, содержащих названия оболочек и показатели экспонент, а сокращенное название элемента
	for (typename map< vector< string> , vector < vector < vector < pair < T, T> > > > >::const_iterator it = content.begin(); it != content.end(); it++){ 
		for ( int i=0; i<int(it->first.size()); i++){
			if(it->first[i]=="!") out<<'\n';
			if(regex_search(it->first[i],elName))
				catchName=it->first[i];
			out<<' '<<it->first[i];
		}
		out<<'\n'; 
		for (int i=0; i<int(it->second.size()); i++){
			uniqueIndex.clear();
			out<<shellName(i)<<" , "<<elementSymbol(catchName)<<" , ";
			search.clear();
			for (int j=0; j<int(it->second[i].size()); j++){
				alreadyExist=false;
				for (int l=0; l<int(it->second[i][j].size()); l++){
					for(unsgd k=0; k<search.size(); k++){
						if(it->second[i][j][l].first==search[k]){
							alreadyExist=true;
							break;
						}
					}
					if(alreadyExist) continue;			
					out<<it->second[i][j][l].first<<" , ";
					search.push_back(it->second[i][j][l].first);
				}


			}
				out<<'\n';
				for(int j=0; j<int(it->second[i].size()); j++){
					coef.clear();
					out<<"c, ";
					for(int l=0; l<int(it->second[i][j].size()); l++){
						uniq=true;
						for(unsgd k=0; k<uniqueIndex.size(); k++){
							if(it->second[i][j][l].first==uniqueIndex[k])
							uniq=false;
						}
						if(!uniq) continue;
						uniqueIndex.push_back(it->second[i][j][l].first);
					}
					for(int l=0; l<int(it->second[i][j].size()); l++){
						coef.push_back(it->second[i][j][l].second);
						for(unsgd t=0; t<uniqueIndex.size(); t++){
							if(it->second[i][j][0].first==uniqueIndex[t]){ 
								from=t;
								findFrom=true;
							}
							if(it->second[i][j][int(it->second[i][j].size()-1)].first==uniqueIndex[t]){
								to=t;
								findTo=true;
							}
						}
						if(!findFrom || !findTo){
							cerr<<"\nОшибка! Не найдены коэффициенты"<<endl;
							return false;
						}
				
					}
					out<<from+1<<"."<<to+1<<" , ";
					for(unsgd a=0; a<coef.size(); a++){
						out<<coef[a]<<" , ";
					}
					out<<'\n';
				}
			out<<'\n';
		}


	}
	out<<"}"<<endl;
	out.close();
	ifstream inp("outputFile");
	ofstream outGood(fileName);
	while(getline(inp,str)){
		if(regex_search(str,a)){
			str=regex_replace(str,a,b);
		}
		outGood<<str<<endl;
	}
	if(remove("outputFile")!=0)
		cerr<<"Файл \"outputFile\" не удален!"<< endl;
	return true;	
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetGamessFormat( const char * fileName) const{
	return true;
}


#endif
