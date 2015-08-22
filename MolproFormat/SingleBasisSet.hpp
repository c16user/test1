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
	if(regex_match(str,regex("\\s*HYDROGEN\\s*"))) return "H";
        if(regex_match(str,regex("\\s*HELIUM\\s*"))) return "HE";
        if(regex_match(str,regex("\\s*LITHIUM\\s*"))) return "LI";
        if(regex_match(str,regex("\\s*BERYLLIUM\\s*")) || regex_match(str,regex("\\s*GLUCINIUM\\s*"))) return "BE";
        if(regex_match(str,regex("\\s*BORON\\s*"))) return "B";
        if(regex_match(str,regex("\\s*CARBON\\s*"))) return "C";
        if(regex_match(str,regex("\\s*NITROGEN\\s*"))) return "N";
        if(regex_match(str,regex("\\s*OXYGEN\\s*"))) return "O";
        if(regex_match(str,regex("\\s*FLUORINE\\s*"))) return "F";
        if(regex_match(str,regex("\\s*NEON\\s*"))) return "NE";
        if(regex_match(str,regex("\\s*NATRIUM\\s*")) || regex_match(str,regex("\\s*SODIUM\\s*"))) return "NA";
        if(regex_match(str,regex("\\s*MAGNESIUM\\s*"))) return "MG";
        if(regex_match(str,regex("\\s*ALUMINIUM\\s*"))) return "AL";
        if(regex_match(str,regex("\\s*SILICON\\s*"))) return "SI";
        if(regex_match(str,regex("\\s*PHOSPHOROUS\\s*"))) return "P";
        if(regex_match(str,regex("\\s*SULFUR\\s*"))) return "S";
        if(regex_match(str,regex("\\s*CHLORINE\\s*"))) return "CL";
        if(regex_match(str,regex("\\s*ARGON\\s*"))) return "AR";
        if(regex_match(str,regex("\\s*POTASSIUM\\s*"))) return "K";
        if(regex_match(str,regex("\\s*CALCIUM\\s*"))) return "CA";
        if(regex_match(str,regex("\\s*SCANDIUM\\s*"))) return "SC";
        if(regex_match(str,regex("\\s*TITANIUM\\s*"))) return "TI";
        if(regex_match(str,regex("\\s*VANADIUM\\s*"))) return "V";
        if(regex_match(str,regex("\\s*CHROMIUM\\s*"))) return "CR";
        if(regex_match(str,regex("\\s*MANGANESE\\s*"))) return "MN";
        if(regex_match(str,regex("\\s*IRON\\s*"))) return "FE";
        if(regex_match(str,regex("\\s*COBALT\\s*"))) return "CO";
        if(regex_match(str,regex("\\s*NICKEL\\s*"))) return "NI";
        if(regex_match(str,regex("\\s*COPPER\\s*"))) return "CU";
        if(regex_match(str,regex("\\s*ZINC\\s*"))) return "ZN";
        if(regex_match(str,regex("\\s*GALLIUM\\s*"))) return "GA";
        if(regex_match(str,regex("\\s*GERMANIUM\\s*"))) return "GE";
        if(regex_match(str,regex("\\s*ARSENIC\\s*"))) return "AS";
        if(regex_match(str,regex("\\s*SELENIUM\\s*"))) return "SE";
        if(regex_match(str,regex("\\s*BROMINE\\s*"))) return "BR";
        if(regex_match(str,regex("\\s*KRYPTON\\s*"))) return "KR";
        if(regex_match(str,regex("\\s*RUBIDIUM\\s*"))) return "RB";
        if(regex_match(str,regex("\\s*STRONTIUM\\s*"))) return "SR";
        if(regex_match(str,regex("\\s*YTTRIUM\\s*"))) return "Y";
        if(regex_match(str,regex("\\s*NIOBIUM\\s*"))) return "NB";
        if(regex_match(str,regex("\\s*MOLYBDENUM\\s*"))) return "MO";
        if(regex_match(str,regex("\\s*TECHNETIUM\\s*"))) return "H";
        if(regex_match(str,regex("\\s*RUTHENIUM\\s*"))) return "RU";
        if(regex_match(str,regex("\\s*RHODIUM\\s*"))) return "RH";
        if(regex_match(str,regex("\\s*PALLADIUM\\s*"))) return "PD";
        if(regex_match(str,regex("\\s*ARGENTUM\\s*"))) return "AG";
        if(regex_match(str,regex("\\s*CADMIUM\\s*"))) return "CD";
        if(regex_match(str,regex("\\s*INDIUM\\s*"))) return "IN";
        if(regex_match(str,regex("\\s*TIN\\s*"))) return "SN";
        if(regex_match(str,regex("\\s*STIBIUM\\s*")) || regex_match(str,regex("\\s*ANTIMONY\\s*"))) return "SB";
        if(regex_match(str,regex("\\s*TELLURIUM\\s*"))) return "TE";
        if(regex_match(str,regex("\\s*IODINE\\s*"))) return "I";
        if(regex_match(str,regex("\\s*XENON\\s*"))) return "XE";
        if(regex_match(str,regex("\\s*CAESIUM\\s*"))) return "CS";
        if(regex_match(str,regex("\\s*BARIUM\\s*"))) return "BA";
        if(regex_match(str,regex("\\s*LANTHANUM\\s*"))) return "LA";
        if(regex_match(str,regex("\\s*CERIUM\\s*"))) return "CE";
        if(regex_match(str,regex("\\s*PRASEODYMIUM\\s*"))) return "PR";
        if(regex_match(str,regex("\\s*NEODYMIUM\\s*"))) return "ND";
        if(regex_match(str,regex("\\s*PROMETHIUM\\s*"))) return "PM";
        if(regex_match(str,regex("\\s*SAMARIUM\\s*"))) return "SM";
        if(regex_match(str,regex("\\s*EUROPIUM\\s*"))) return "EU";
        if(regex_match(str,regex("\\s*GADOLINIUM\\s*"))) return "GD";
        if(regex_match(str,regex("\\s*TERBIUM\\s*"))) return "TB";
        if(regex_match(str,regex("\\s*DYSPROSIUM\\s*"))) return "DY";
        if(regex_match(str,regex("\\s*HOLMIUM\\s*"))) return "HO";
        if(regex_match(str,regex("\\s*ERBIUM\\s*"))) return "ER";
        if(regex_match(str,regex("\\s*THULIUM\\s*"))) return "TM";
        if(regex_match(str,regex("\\s*YTTERBIUM\\s*"))) return "YB";
        if(regex_match(str,regex("\\s*LUTECIUM\\s*"))) return "LU";
        if(regex_match(str,regex("\\s*HAFNIUM\\s*"))) return "HF";
        if(regex_match(str,regex("\\s*TANTALUM\\s*"))) return "TA";
        if(regex_match(str,regex("\\s*TUNGSTEN\\s*"))) return "W";
        if(regex_match(str,regex("\\s*RHENIUM\\s*"))) return "RE";
        if(regex_match(str,regex("\\s*OSMIUM\\s*"))) return "OS";
        if(regex_match(str,regex("\\s*IRIDIUM\\s*"))) return "IR";
        if(regex_match(str,regex("\\s*PLATINUM\\s*"))) return "PT";
        if(regex_match(str,regex("\\s*GOLD\\s*"))) return "AU";
        if(regex_match(str,regex("\\s*MERCURY\\s*")) || regex_match(str,regex("\\s*QUICKSILVER\\s*"))) return "HG";
        if(regex_match(str,regex("\\s*THALLIUM\\s*"))) return "TL";
        if(regex_match(str,regex("\\s*LEAD\\s*"))) return "PB";
        if(regex_match(str,regex("\\s*BISMUTH\\s*"))) return "BI";
        if(regex_match(str,regex("\\s*POLONIUM\\s*"))) return "PO";
        if(regex_match(str,regex("\\s*ASTATINE\\s*"))) return "AT";
        if(regex_match(str,regex("\\s*RADON\\s*"))) return "RN";
        if(regex_match(str,regex("\\s*FRANCIUM\\s*"))) return "FR";
        if(regex_match(str,regex("\\s*RADIUM\\s*"))) return "RA";
        if(regex_match(str,regex("\\s*ACTINIUM\\s*"))) return "AC";
        if(regex_match(str,regex("\\s*THORIUM\\s*"))) return "TH";
        if(regex_match(str,regex("\\s*PROTACTINIUM\\s*"))) return "PA";
        if(regex_match(str,regex("\\s*URANIUM\\s*"))) return "U";
        if(regex_match(str,regex("\\s*PLUTONIUM\\s*"))) return "NP";
        if(regex_match(str,regex("\\s*AMERICIUM\\s*"))) return "PU";
        if(regex_match(str,regex("\\s*CURIUM\\s*"))) return "CM";
        if(regex_match(str,regex("\\s*BERKELIUM\\s*"))) return "BK";
        if(regex_match(str,regex("\\s*CALIFORNIUM\\s*"))) return "CF";
        if(regex_match(str,regex("\\s*EINSTEINIUM\\s*"))) return "ES";
        if(regex_match(str,regex("\\s*FERMIUM\\s*"))) return "FM";
        if(regex_match(str,regex("\\s*MENDELEVIUM\\s*"))) return "MD";
        if(regex_match(str,regex("\\s*NOBELIUM\\s*"))) return "NO";
        if(regex_match(str,regex("\\s*LAWRENCIUM\\s*"))) return "LR";
        if(regex_match(str,regex("\\s*RUTHERFORDIUM\\s*"))) return "RF";
	return "EN";

}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetMolproFormat( const char * fileName) const{
		ofstream out("outputFile");
		out.precision(7);
		out.setf(std::ios::fixed, std::ios::floatfield);
		vector <double> search;
		vector <double> uniqueIndex;
		vector <double> coef;
		unsgd from=0;
		unsgd to=0;
		string str, str1;
		string catchName="";
		regex a("\\s*,\\s*$");
		regex b("");
		regex q("^\\s*");
		regex elName("[a-zA-Z]{4,}");
		bool alreadyExist=false;
		bool findFrom=false;
		bool findTo=false;
		bool uniq=true;
		out<<"basis=\{";
	for (typename map< vector< string> , vector < vector < vector < pair < T, T> > > > >::const_iterator it = content.begin(); it != content.end(); it++){ 
		for ( int i=0; i<int(it->first.size()); i++){
			if(it->first[i]=="!") out<<'\n';
			if(regex_search(it->first[i],elName))
				catchName=elementSymbol(it->first[i]);
			out<<' '<<it->first[i];
		}
		out<<'\n'; 
		for (int i=0; i<int(it->second.size()); i++){
			uniqueIndex.clear();
			out<<shellName(i)<<", "<<catchName<<" , ";
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
					out<<it->second[i][j][l].first<<", ";
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
							if(it->second[i][j][t].first==uniqueIndex[t]){ 
								from=t;
								findFrom=true;
							}
							if(it->second[i][j][t].first==uniqueIndex[t]){
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
						out<<coef[a]<<", ";
					}
					out<<'\n';
				}
		}


	}
	out<<"}"<<endl;
	out.close();
	ifstream inp("outputFile");
	ofstream outGood("basis");
	while(getline(inp,str)){
		if(regex_search(str,a)){
			str=regex_replace(str,a,b);
		}
		//if(regex_search(str,q)){
		//	str=regex_replace(str,q,b); //не получается почему то сразу в одном все сделать
		//}
		outGood<<str<<endl;
	}
	inp.close();
	outGood.close();
	ifstream inp1("basis");
	ofstream Final(fileName);
	while(getline(inp1,str1)){
		if(regex_search(str1,q))
			str1=regex_replace(str1,q,b);
		Final<<str1<<endl;
	}
	if(remove("outputFile")!=0)
		cerr<<"Внимание! Файл \"outputFile\" не удален!"<< endl;
	if(remove("basis")!=0)
		cerr<<"Внимание! Файл \"basis\" не удален!"<<endl;

	system("/home/chesnykh/Documents/work/progs/test1/MolproFormat/BasisScript.sh");
	return true;	
}

template< typename T >
bool SingleBasisSet<T>::exportBasisSetGamessFormat( const char * fileName) const{
	return true;
}


#endif
