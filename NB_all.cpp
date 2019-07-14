#include<iostream>
#include<fstream>
#include<string>
#include<list> 
#include<algorithm>
#include<cmath>
#include<map> 
#include <vector>
#include<sstream> 
using namespace std;
/*回归*/ 
fstream  nbresult("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\nbhuigui_result_zhenghe.csv",ios::out);
//fstream  tf("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\train_tf.csv",ios::out);

map <string,int> wordvector;
double trainmatrix [1001][3008] ={0};
double trainmatrix1 [6][3003] ={0};//3000:单词总数量  3001：joy出现的次数 3002：不重复单词的总数 
int COL = 0; 
int ROW = 0;
int SUM = 0;
//切割一个行词汇，构成词汇向量 
/*回归*/ 
void creatvector(string all,string label){
	map <string,int>::iterator it;
	stringstream ss,ss_l;
	ss<<all;
	ss_l<<label;
	string tmp;
	while(ss>>tmp){
		it = wordvector.find(tmp);
		if(it == wordvector.end()){
			wordvector.insert(pair<string,int>(tmp,COL));
			trainmatrix[ROW][3007]++;
			COL++;
		} 
	    int col = wordvector[tmp];
	   // trainmatrix[1000][col]++;//该单词第几次出现 
	    trainmatrix[ROW][col]++;
	    trainmatrix[ROW][3000]++;//第row行的单词总数 
	}
    char ttmp;
    for(int i =0;i<6;i++){
    	ss_l>>ttmp;
    	ss_l>>trainmatrix[ROW][3001+i];//第row行的概率 
    }
	 ROW++; //一共几行 
}
/*分类*/ 
void creatvector1(string all,int row){
	map <string,int>::iterator it;
	stringstream ss;
	ss<<all;
	string tmp;
	while(ss>>tmp){
		it = wordvector.find(tmp);
		if(it == wordvector.end()){
			wordvector.insert(pair<string,int>(tmp,COL));
			trainmatrix[row][3002]++; 
			COL++;
		} 
	    int col = wordvector[tmp];
	    trainmatrix[row][col]++;
	    trainmatrix[row][3000]++;//某个心情 所有单词出现次数 
	}
	 trainmatrix[row][3001] ++; //某行心情出现的次数 
	 SUM++;//一共几行 
}	
/*回归*/ 
void predict(string a){
	double b = 0.01;
	map <string,int>::iterator it;
	stringstream ss;
	ss<<a;
	string tmp;
	double p[6]={0,0,0,0,0,0};
	int word_col[1000]={0};
	int col_1 = 0;
	while(ss>>tmp){
		it = wordvector.find(tmp);
		if(it == wordvector.end()) continue;//不存在这个单词 
		word_col[col_1] = wordvector[tmp];//存储这些单词出现在哪一列 
		col_1++;	
	}
	for(int i = 0;i<ROW;i++){//跟每一行做比较 
		//double p_tmp[6]={1,1,1,1,1,1};
		double word_p =1; 
		for(int j = 0;j<col_1;j++){//找出每个单词的概率 相乘 
			word_p = word_p * (trainmatrix[i][word_col[j]]+b)/(b*COL+trainmatrix[i][3000]);
		}
		for(int k =0;k<6;k++){
		    p[k] +=  word_p * ((double)trainmatrix[i][3001+k]);
		}
	}
	double sum = p[0]+p[1]+p[2]+p[3]+p[4]+p[5];
	for(int k =0;k<6;k++){
		nbresult<<p[k]/sum<<',';
	}
	nbresult<<endl;
}
/*分类*/ 
int predict1(string a){
	b = 0.53; 
	map <string,int>::iterator it;
	stringstream ss;
	ss<<a;
	string tmp;
	//double p[6]={0,0,0,0,0,0};
	double p[6]={1,1,1,1,1,1};
	while(ss>>tmp){
		it = wordvector.find(tmp);
		if(it == wordvector.end()) continue;
		int col = wordvector[tmp];
		for(int i=0;i<6;i++){
			p[i] = p[i]+log2((double)(trainmatrix[i][col]+b))-log2(double(trainmatrix[i][3000]+COL*b));
			//cout<<"trainmatrix[i][col]  "<<trainmatrix[i][col]+1<<endl;
			//cout<<trainmatrix[i][3002]<<endl;
		}		    //trainmatrix[i][3002]
	}
	double max = p[0] + log2((double)trainmatrix[0][3001]/(double)SUM);
	int index = 0;
	for(int i=1;i<6;i++){
		p[i]  = p[i] +log2((double)trainmatrix[i][3001]/(double)SUM);
		//cout<<"trainmatrix[i][3002]  "<<trainmatrix[i][3002]<<endl; 
		if(max<p[i]){
			max = p[i];
			index = i;
		}
		//cout<<"i  "<<p[i]<<endl;
	}
	return index;
}

void huigui(){
	nbresult<<endl;
	fstream  train("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\train_set.csv");
	string a ;
	getline(train,a);//废掉第一行 
	while(getline(train,a)){
		size_t pos = a.find(',');  //找出第二个制表符的位置 
		string label = a.substr(pos);
		int len =  (a.substr(pos)).size();		
		a = ' '+a.substr(0,a.size()-len)+' '; // 在内容的前后加入空格，以便每个单词的分割 
		creatvector(a,label);              //将行内容进行分割，创建词汇列表            //将行内容进行分割，创建词汇列表 
	}
	//cout<<"Onehot finish"<<endl;
	fstream  valid("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\validation_set.csv");
	
	
	int sum = 0;
	getline(valid,a);
	while(getline(valid,a)){
		sum++;
		size_t pos = a.find(',');  //找出第二个制表符的位置 
		//string label = substr(pos);//后面是label
		int len =  a.substr(pos).size();	
		a = ' '+a.substr(0,a.size()-len)+' '; // 在内容的前后加入空格，以便每个单词的分割
		 predict(a);
    }
	//cout<<sum<<endl;
} 

void fenlei(){
    map <string,int> mood;
	mood["anger"] = 0;
	mood["disgust"] = 1;
	mood["fear"] = 2;
	mood["joy"] = 3;
	mood["sad"] = 4;
	mood["surprise"] = 5;
	fstream  train("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\train_set.csv");
	//fstream  nbresult1("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\nb_result.csv",ios::out);
	string a ;
	getline(train,a);//废掉第一行 
	while(getline(train,a)){
		size_t pos = a.find(',');  //找出第二个制表符的位置 
		int row = mood[a.substr(pos+1)];//找出对应哪个心情 
		int len =  (a.substr(pos)).size();		
		a = ' '+a.substr(0,a.size()-len)+' '; // 在内容的前后加入空格，以便每个单词的分割 
		creatvector1(a,row);              //将行内容进行分割，创建词汇列表            //将行内容进行分割，创建词汇列表 
	}
	cout<<"Onehot finish"<<endl;
	
	fstream  valid("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\validation_set.csv");
	getline(valid,a);
	int TRUE = 0;
	int sum = 0;
	while(getline(valid,a)){
		sum++;
		size_t pos = a.find(',');  //找出第二个制表符的位置 
		int label_now = mood[a.substr(pos+1)];//后面是label
		int len =  a.substr(pos).size();	
		a = ' '+a.substr(0,a.size()-len)+' '; // 在内容的前后加入空格，以便每个单词的分割
		int lable_pre = predict1(a);
		//nbresult1<<lable_pre<<endl;
		if(lable_pre == label_now) 
			TRUE ++;
	}
	cout<<TRUE<<endl;
	cout<<sum<<endl;
}
int main(){
	huigui();
	cout<<"huigui finish"<<endl; 
	fenlei();
	return 0;
} 



