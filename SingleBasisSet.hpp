#ifndef SINGLEBASISSET_H
#define SINGLEBASISSET_H

#include<map>
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<boost/regex.hpp>
//#include <regex>

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
using boost::regex_replace;
using boost::regex_match;
using boost::regex_search;
using boost::regex;

template< typename T >
class SingleBasisSet{
private:
	map< vector< string> , vector < vector < vector < pair < T, T> > > > > content;
	bool getLandNoPrimitives(string str, int & L, int & N) const;
//	непонятно, надо имя или нет tring//значение пары: внешний вектор-его индексация-номер орб кв числа, промежуточный вектор-оболочка(одна), внутренний вектор-вектор пар-показатель экспоненты-коэффициент при экспоненте.  
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


regex shell("^[A-Z]\\s{1,3}\\d{1,2}");//оболочка
bool shellsearch (string str){
	if (regex_search(str,shell)) return true;
}
template< typename T >
bool SingleBasisSet<T>::getLandNoPrimitives(string str, int & L, int & N) const{
	regex shell("^\\s*([a-zA-Z])\\s+(\\d+)$");
	boost::smatch result;
/*	
	Идея следующая.
	1. Проверить, что строка отвечает регулярному выражению вида ^\s*([a-zA-Z])\s+(\d+)$', если все плохо - вернуть false
	2. Проверить, что результат один - это s или S (хоть прямым if'ом) - что распознается, иначе вернуть false
	3. для S - L=0, P - L=1, D L=2, ..... L = -1;
	4. Результат два просто передать в N через atoi. Если N=0 - выразить пользователю недоумение.
	5. вернуть true;
*/
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
	if (lTmp==-2) return false;
	N=atoi((result[2].str()).c_str());
	if (N==0) return false;
	L=lTmp;
	return true;
}


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
	int *index = new int;
	int i = 0;
	vector<pair<double,double> > CiAi;
	regex empty("^\\s*$");//пустая строка
	regex emptyOrComment("^\\s*(!.*)*$");//пустая строка или коммент. (все ли правильно в этом рв?)
	regex firstLine("^\\s*\\$DATA\\s*$");//$DATA
	regex del("[A-Z]{4,}\\s*$");//название элемента. здесь вопрос-в рег выражении оказывается что друг за другом следуют подряд 4 или больше букв, в названии любого элемента это действительно присутствует, но это рег. выражение не контролирует случай, если последовательность букв не отвечает названию какого-либо элемента. 
	regex shell("^\\s*([a-zA-Z])\\s+(\\d+)$");//оболочка
	regex number("^\\s*(\\d+)((\\s*\\d+[\\.]?\\d+[EeDd]?[+-]?\\d+)*)(.*)");
	regex numer("\\d+[\\.]?\\d+[EeDd]?[+-]?\\d+");
	regex trash1("^[A-Z]{4,}\\d+.*");
	regex trash2("^[A-Z]{4,}\\s*\\w*.*$");
	regex end("^\\$END");
//	Проверка шапки файла
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
/* Пример разбора строки
	getline(inp,str);

	const boost::sregex_iterator itEmpty;
	boost::sregex_iterator it(str.begin(),str.end(),regex("\\s*[^\\s]+"));
	elementLabel.clear();
	while (it != itEmpty ) elementLabel.push_back((*it++).str());
	content[elementLabel]=elementContent;
	return true;
*/



//	Заполнение элементов
	regex pars("\\s*[^\\s]+");
	regex el("^[A-Z]{4,}.*$");
	regex partComment("^.*!.*$");
	regex trash("!.*$");
	regex clear("");
	//regex start("^\\s+\\d{1,2}.*");	
	regex NoLShell("^\\s+\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+$");//не L-оболочка
	regex LShell("^\\s+\\d+\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+(-?\\d+\\.?\\d+[E|D]?[-|+]?\\d{0,})\\s+$");
	boost::smatch result;
	boost::smatch CiAiNoL;
	boost::smatch CiAiL;
	int L=-3;
	int N;
	double Ci, Ai, mix;
	while(getline(inp,str)) {
		noLine++;
		if (regex_search(str, empty) || regex_search(str,emptyOrComment)) continue;
		if (regex_search(str,partComment)){
			str=regex_replace(str,trash,clear);//отброс комментариев в строке
			//cout << str << endl;
		}
		if(regex_search(str,el)){
			boost::sregex_iterator it(str.begin(),str.end(),pars);
			boost::sregex_iterator itbad;//этот if-блок-парсинг строки с названием элемента.
			while (it != itbad){
			elementLabel.push_back((*it++).str());
			}
			//copy(elementLabel.begin(),elementLabel.end(),std::ostream_iterator<string>(cout,"<--"));
			//elementLabel.clear();		
			cout << endl;
			continue;
		}
		if (regex_search(str,result,shell)){
			N = atoi((result[2].str()).c_str());
			if(getLandNoPrimitives(str,L,N)){
				//cout << L << " and " << N << endl;
				continue;
			}
			if(!getLandNoPrimitives(str,L,N)){
				cout << "!!!!!!!!!!";
				break;
			}
			/*else {
				//cout << "В строке " << noLine << "\""<<str<<"\"" << " ошибка!" << endl;
				continue;//не получается зайти в else-блок
			}*/
		}
		if (regex_search(str,CiAiNoL,NoLShell) && (L != -1)){
				for (int i=0; i<N; i++){
					Ci=atof((CiAiNoL[1].str()).c_str());
					Ai=atof((CiAiNoL[2].str()).c_str());
					cout << Ci << " " << Ai << endl;
					if (i != N-1)
					getline(inp,str);
				}
				cout << endl;
		}
		if(regex_search(str,CiAiL,LShell) && L == -1){
			for(int i=0; i<N; i++){
				Ci=atof((CiAiL[1].str()).c_str());
				Ai=atof((CiAiL[2].str()).c_str());
				mix=atof((CiAiL[3].str()).c_str());
				cout << Ci << " " << Ai << " " << mix << endl;
				if (i != N-1)
				getline(inp,str);
			}
			cout << endl;
		}
			

	}
	if(false)
	cout << "В строке " << noLine << " \""<<str<<"\"\n" << "ошибка, не существует химического элемента с таким названием, чтениефайла не будет продолжено" << endl;  

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
	delete [] index;
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

