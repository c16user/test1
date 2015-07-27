#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <boost/regex.hpp>
#include <cstdlib>
#include <map>
#include <utility>
#include <regex>
using std::pair;
using std::make_pair;
using boost::regex;
using boost::regex_replace;  // странно, но предыдущий неймлист ее включает
using std::string;
using std::smatch;
int main(int argc, char** argv)
{
	std::ifstream inp("HCuN.basis");
	std::ofstream out("output.txt");
	std::string str;
	regex a("!");//комменты в начале файла
	regex b("((\\w|\\s){3,}?)((\\d|\\s){2,}?).*");//содержательная часть файла
	regex name("\\w{4,}");//полные названия элементов и числа
	regex data("\\$DATA");
	regex end("\\$END");
	regex del("[A-Z]{4,}");//название элемента
	string empt = "";
	regex head("^\\w\\s{3}\\d{1,2}");
	regex numbers ("\\s\\d{1,2}\\s*\\d*\\.\\s*.*");
	regex ignore("^[A-Z]\\s{1,3}\\d{1,2}");
	regex start("\\s\\d{1,2}.*");//первые символы строк, которые надо вытащить
	string str_1st;
	string str_2nd;
	string str_3rd;
	string str_4th;
	regex first("\\s*\\d*\\..*");//это и ниже-регулярные выражения для вывода на экран только определенного столбца из файла
	regex second_1("^\\s{1,2}\\d{1,2}\\s{2,}");
	regex second_2("\\s{5,}.*");
	regex third_1("^\\s{1,2}\\d{1,2}\\s{2,}\\d*\\.\\d*\\s{5,}");
	regex third_2("\\s{5,}(-|\\d*).*$");
	regex fourth_1("^(-|\\d{1})\\d{1,}\\.\\d{1,}(E|D)?-?\\d{1,}");
	regex numer("\\d+[\\.]?\\d+[EeDd]?[+-]?\\d+");
	regex all(".*");


	std::cout.precision(10);
	//out.setf(std::ios::fixed,std::ios::floatfield); //форматированный вывод в файл
	std::cout.setf(std::ios::fixed,std::ios::floatfield);
	std::vector<int> arr1;//нумерация (она нужна?)
	std::vector<string> help_vector; //вспомогательный вектор для хранения строк вида "S   3".
	std::vector<double> vector_level1;//строки с числами
	std::vector<std::vector <double> > vector_level2;//вектор строк с числами
	std::map<string,std::vector<std::vector<double> > > map_level3;//карта-первый элемент-строка, соответствующая рег выражению "ignore" т.е. строка вида "S   3", второй элемент карты-вектор vector_level2.
	std::vector<std::map<string,std::vector<std::vector<double> > > > vector_level4; //вектор 4 уровня-собираем все данные об элементе
	std::vector<std::vector<std::map<string,std::vector<std::vector<double> > > > > vector_allfile; //весь файл
	while(std::getline(inp,str)){
		if(((regex_search(str,b)) || regex_search(str,name)) && !((regex_search(str,a))||regex_search(str,data))){//это первый вариант, выводятся названия элементы а дальше сплошняком числа
			if((regex_search(str,del)) || (regex_search(str,end))){
				vector_allfile.push_back(vector_level4);
				if(!vector_level4.empty())
				vector_level4.clear(); 
				//std::cout << str << std::endl;
				continue;
			}
			if(regex_search(str,ignore)){
				help_vector.push_back(str);
				if(vector_level2.empty()){
					continue;
				}
				std::cout<<str<<std::endl;
				map_level3.insert(std::make_pair(help_vector[help_vector.size()-1],vector_level2));
				vector_level4.push_back(map_level3);
				map_level3.clear();
				vector_level2.clear();	
				continue;
			}
			if(regex_search(str,start)){
//				std::cout<<str<<'\n';
				boost::smatch ourRes;
		if(regex_search(str,ourRes,regex("^\\s*(\\d+)((\\s*\\d+[\\.]?\\d+[EeDd]?[+-]?\\d+)*)(.*)"))){
					// TODO ourRes[3] проверить ....
					/*std::string str3(ourRes[3]);
					boost::sregex_iterator It(str3.begin(),str3.end(), all);
					boost::sregex_iterator InvalidIt;
					while (It != InvalidIt){
					if ((((*It++).str()).c_str())!="\0" //или...)
						std::cout << "INPUT FILE INCORRECT\n";
						exit(-1);
					}*///подумать как правильео сделать условие на проверку "непустоты" подстроки, следующей за последней цифрой последнего числа в данной строке 
					std::cout<<"---"<<ourRes[2]<<'\n';
					std::string str2(ourRes[2]);
					smatch::iterator it;
					//char* res = (char*)str2.c_str();
					boost::sregex_iterator xIt(str2.begin(), str2.end(), numer);
					boost::sregex_iterator xInvalidIt;
					while(xIt != xInvalidIt){
						//std::cout << (*xIt++).str() << " ";	
						vector_level1.push_back(atof(((*xIt++).str()).c_str()));
						//std::cout << level1.back() << " ";
					}
					vector_level2.push_back(vector_level1);
					vector_level1.clear();
					std::cout<<'\n';
				}


			}
		}

	}
}





