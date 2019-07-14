#include<iostream>
#include<fstream>
#include<string>
#include<list> 
#include<algorithm>
#include<cmath>
#include<map> 
#include<vector>
#include<sstream>
using namespace std;

struct node{
	double mood[6];
	node(){
		mood[0]=mood[1] = mood[2]=mood[3] =mood[4]=mood[5] =0;
	}
	node& operator =(const node &other){
		for(int i =0;i<6;i++ ) mood[i] = other.mood[i];
		return *this; 
	}
	node& operator +=(const node &other){
		for(int i =0;i<6;i++ ) mood[i] = mood[i]+other.mood[i];
		return *this; 
	}
	node& operator /(double other){
		for(int i =0;i<6;i++ ) mood[i] = mood[i]/other;
		return *this; 
	}
		node& operator *(double other){
		for(int i =0;i<6;i++ ) mood[i] = mood[i]*other;
		return *this; 
	}
};

list <string> lineword;
list <string> wordvector;
/*����*/ 
vector <string> labeltrain;
vector <string> labelvalidation; 
/*�ع�*/ 
vector <node> labeltrain1;
vector <node> labelvalidation1; 

//ÇÐ¸îÒ»¸öÐÐ´Ê»ã£¬¹¹³É´Ê»ãÏòÁ¿ 
/*����*/ 
struct intstring{
		double dis;
		string label;
};
/*�ع�*/
struct dislabel{
	double dis;
	struct node label;
};
/*����*/
bool cmp(const intstring &a,const intstring&b)
{
    return a.dis > b.dis;//´Ó´óµ½Ð¡ 
}
/*�ع�*/
bool cmp1(const dislabel &a,const dislabel&b)
{
    return a.dis > b.dis;//´Ó´òµ½Ð¡  
}
/*����*/
//Map为与训练集每行文本的余弦相似度
string labelpre(vector<intstring> Map,int k){
    if(Map[0].dis == 1||Map[0].dis == 0) return Map[0].label;
    //dis=1，两个文本完全一致，直接返回label
    //dis=0,预测的文本的单词在训练集中都没出现过，无法预测，返回第一个文本的label
	map<string,double> MMap;//MMap存储的是每个类别的内容与之权重（距离的加和）
	double max = 0; 
	string labelpredic = "";
	map<string,double>::iterator iter;
	
	for(int i=0;i<k;i++){
		iter = MMap.find(Map[i].label);//查找这个类别的迭代器
		if(iter!=MMap.end()) iter->second += Map[i].dis ;//如果出现过，则权重加上这次的距离
		else  MMap.insert(pair<string,double>(Map[i].label,Map[i].dis));
		//没出现过，加入这个类别与之距离
		iter = MMap.find(Map[i].label);
		if(max < iter->second){//查找权重最大的类别，就是预测的类别
			max = iter->second;
			labelpredic = iter->first;
		}
	}
	/* 
	for(int i=0;i<k;i++){
		iter = MMap.find(Map[i].label);
		if(iter!=MMap.end()) iter->second++;
		else  MMap.insert(pair<string,int>(Map[i].label,1));	
		iter = MMap.find(Map[i].label);
		if(max < iter->second){
			max = iter->second;
			labelpredic = iter->first;
		}
    }*/ 
	return labelpredic;
}
/*�ع�*/
node labelpre1(vector<dislabel> Map,int k){
    if(Map[0].dis == 1)return Map[0].label;
    //相似度等于1，完全一样的两个样本，返回一样的概率
	node fenzi;
	if(Map[0].dis == 0 ){
		fenzi.mood[0]=fenzi.mood[1]=fenzi.mood[2]
		=fenzi.mood[3]=fenzi.mood[4]=fenzi.mood[5] 
		=double(1)/double(6);
		return fenzi;
	}
	//相似度等于0，无法预测，每个概率分配均等，1/6
	for(int i=0; i<k ; i++)
		fenzi += Map[i].label * Map[i].dis;//距离*对应的概率
	double sum = fenzi.mood[0]+fenzi.mood[1]+fenzi.mood[2]
	+fenzi.mood[3]+fenzi.mood[4]+fenzi.mood[5]; //概率总和
	return fenzi/sum;//概率归一化
}

/*����*/ 
void creatvector(string all){
	stringstream ss;
	ss<<all;
	string s;
	while(ss>>s){
		list<string>::iterator it = find(wordvector.begin(),wordvector.end(),s);
		if(it == wordvector.end()) //Ö®Ç°Ã»³öÏÖ¹ýµÄµ¥´Ê£¬¼ÓÈë´Ê»ãÏòÁ¿ÖÐ 
			wordvector.push_back(s);	
	}
}	
/*����*/
//È·¶¨ÔÚ×Ö·û´®ÖÐstrÖÐ×Ó´®sub³öÏÖµÄ´ÎÊýnum 
int fun3(string str,string sub)
{
	int num = 0;
	while(1){
		size_t pos = str.find(sub);
  		if (pos != -1 )  num++;
  		else break;
  	    str = str.substr(pos+sub.size()-1); //È¥µôº¬ÓÐ×Ó´®subµÄ²¿·Ö£¬¼ÌÐø²éÕÒ×Ó´®sub 
	}
	return num;
}
void  fenlei(){
	fstream  train("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\train_set.csv");
	//fstream  testidf("D:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\testidf_set.csv",ios::out);
	//fstream  TFIDF("D:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\TFIDF.csv",ios::out);
	
	string a ;
    int k;
	//ÇÐ¸îµ¥´Ê£¬ÐÎ³ÉÁÐ±í 
	getline(train,a);//·ÏµôµÚÒ»ÐÐ 
	while(getline(train,a)){
		size_t pos = a.find(',');  //ÕÒ³öµÚ¶þ¸öÖÆ±í·ûµÄÎ»ÖÃ 
		labeltrain.push_back(a.substr(pos+1));//ºóÃæÊÇlabel
		int len =  (a.substr(pos)).size();		
		a = ' '+a.substr(0,a.size()-len)+' '; // ÔÚÄÚÈÝµÄÇ°ºó¼ÓÈë¿Õ¸ñ£¬ÒÔ±ãÃ¿¸öµ¥´ÊµÄ·Ö¸î 
		lineword.push_back(a);       //½«ÐÐÄÚÈÝ¼ÓÈëÐÐ´Ê»ãÁÐ±íÖÐ 
		creatvector(a);              //½«ÐÐÄÚÈÝ½øÐÐ·Ö¸î£¬´´½¨´Ê»ãÁÐ±í 
	}
	
	int ROW = lineword.size();
	int COL = wordvector.size();
	
	double **Onehot;
	Onehot = new double *[ROW+1];
	
	for(int i = 0; i < ROW+1; i++)
		Onehot[i] = new double [COL+1];
		
	for(int i = 0;i<ROW+1;i++)
	   for(int j = 0;j<COL+1;j++)
	   		Onehot[i][j] = 0;
			    
	int col = 0;
	int row = 0;
	list<string>::iterator it1;
	list<string>::iterator it2;
	//¶ÔÃ¿Ò»ÐÐ´Ê»ã×ö´¦Àí£¬¿´ÊÇ·ñ³öÏÖ´Ê»ãÏòÁ¿ÀïµÄµ¥´Ê£¬ÒÔ¼°³öÏÖµÄ´ÎÊý 
	for(it1 = lineword.begin();  it1 != lineword.end(); it1++){
		 string  linew = *it1; 
		 col=0;
		 for(it2 = wordvector.begin();  it2 != wordvector.end(); it2++){
		 	string vec = *it2;
		 	vec =' '+vec+ ' ';
		 	int num = fun3(linew,vec);
		 	Onehot[row][col] = num;
		 	Onehot[row][COL] += num;
			if(num)  Onehot[ROW][col]++; 
		    col++;
		 }
		 row++;
	} 
	
	for(int i=0 ;i <ROW;i++){
		for(int j =0 ;j<COL;j++){
			Onehot[i][j] = double(Onehot[i][j])/double(Onehot[i][COL])*log2(double(ROW)/double(1+Onehot[ROW][j])); 
		}
	}
	cout<<"tfidf finish"<<endl;
	
	int True = 0;
	int Max = 0;
	int Maxk = 0;
	

	int sum = 0;
    fstream  valid("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\classification_dataset\\validation_set.csv");
	getline(valid,a);
	vector <double*> validonehot1;
	

	
	while(getline(valid,a)){
		sum++;
		size_t pos = a.find(',');  //ÕÒ³öµÚ¶þ¸öÖÆ±í·ûµÄÎ»ÖÃ 
		labelvalidation.push_back(a.substr(pos+1));//ºóÃæÊÇlabel
		int len =  a.substr(pos).size();	
		a = ' '+a.substr(0,a.size()-len)+' '; // ÔÚÄÚÈÝµÄÇ°ºó¼ÓÈë¿Õ¸ñ£¬ÒÔ±ãÃ¿¸öµ¥´ÊµÄ·Ö¸î
		stringstream ss;
		ss<<a;
		string tmp;
		int wordsum = 0;
		while(ss>>tmp)  wordsum++;
		double *line = new double[COL];
		int coli = 0;
		 for(it2 = wordvector.begin();  it2 != wordvector.end(); it2++){	 	
			 	 string vec = *it2;
			 	 vec =' '+vec+ ' ';
			 	 int num = fun3(a,vec);
			 	 line[coli] = (double)num/double(wordsum)*log2(double(ROW)/double(1+Onehot[ROW][coli]));
			 	 coli++;
		}
		validonehot1.push_back(line);//µÃµ½Ã¿Ò»ÐÐµÄÖµ 
		//¸úÑµÁ·¼¯Ã¿Ò»ÐÐ×ö±È½Ï 
	}

	 cout<<1<<endl;
	 cout<<"validonehot1.size() "<<validonehot1.size()<<endl;
   for(int i = 0;i<validonehot1.size();i++){
   	  	double *line = validonehot1[i];
     	vector <intstring> Map; 
		for(int row = 0; row<ROW ; row++){
			double leng = 0;
			double sum1 =0;//某个训练文本的长度的平方
			double sum2 = 0;//当前被测文本的长度平方
			 for(int col = 0; col<COL ; col++)
			 {
			 	sum1+=Onehot[row][col]*Onehot[row][col];//训练文本距离计算
			 	sum2+=line[col]*line[col];//测试文本距离计算
				leng +=  Onehot[row][col]*line[col];//A点乘B的计算
			 }
			 if(sum2 == 0) sum2 = 1;//考虑到分母为0会报错，使之的关于1
			 leng = leng/(double)sqrt(sum1*sum2);//leng就是余弦相似度

			 intstring tmp;
			 tmp.dis = leng;
			 tmp.label = labeltrain[row];
			 Map.push_back(tmp);//µÃµ½ÓëÃ¿Ò»ÐÐµÄ¾àÀë 
		}
	//µÃµ½ÁËÓëÃ¿Ò»ÐÐÖ®¼äµÄÅÅÃû£»
		sort(Map.begin(),Map.end(),cmp);
		string labelpredict = labelpre(Map,11);//99  138    11 151
		/********************************************************/
		if(labelvalidation[i] == labelpredict)
		  True++;
	//cout<<"labelnow == "<<labelnow <<endl<< " predict :     "<<labelpredict<<endl;
	}
	 cout<<1<<endl;
			if(Max<True){
				Max = True;
				Maxk = k;
			}
			//cout<<"k  "<<k<<"   "; 
			cout<<"׼ȷ����  "<<double(True)<<"   "<<double(sum)<<endl;
			cout<<"׼ȷ��"<< double(True)/double(sum)<<endl;
			valid.close();
			True = 0;

	for(int i = 0; i<ROW; i++)
	   delete [] Onehot[i];
	
	train.close();	
	//test.close();	
} 
void huigui(){
	fstream  train("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\train_set.csv");
	fstream  OneHot("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\OneHot_all.csv",ios::out);
	fstream  test("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\test_set_all.csv",ios::out);
	test<<endl;
	string a ;
	int k;
	//ÇÐ¸îµ¥´Ê£¬ÐÎ³ÉÁÐ±í 
	getline(train,a);//·ÏµôµÚÒ»ÐÐ 
	while(getline(train,a)){
		size_t pos = a.find(',');  //ÕÒ³öµÚ¶þ¸öÖÆ±í·ûµÄÎ»ÖÃ 
		stringstream ss1;
		ss1<<a.substr(pos);//°üÀ¨ÁË£¬ºÃ 
	    node tmp;
	    char ttmp;
		for(int i =0;i<6;i++){
			ss1>>ttmp;
			ss1>>tmp.mood[i];
		}
		labeltrain1.push_back(tmp);//ºóÃæÊÇlabel
		
		int len =  (a.substr(pos)).size();		
		a = ' '+a.substr(0,a.size()-len)+' '; // ÔÚÄÚÈÝµÄÇ°ºó¼ÓÈë¿Õ¸ñ£¬ÒÔ±ãÃ¿¸öµ¥´ÊµÄ·Ö¸î 
		lineword.push_back(a);       //½«ÐÐÄÚÈÝ¼ÓÈëÐÐ´Ê»ãÁÐ±íÖÐ 
		creatvector(a);              //½«ÐÐÄÚÈÝ½øÐÐ·Ö¸î£¬´´½¨´Ê»ãÁÐ±í 
	}
	
	int ROW = lineword.size();
	int COL = wordvector.size();

   double **Onehot;
	Onehot = new double *[ROW+1];
	
	for(int i = 0; i < ROW+1; i++)
		Onehot[i] = new double [COL+1];
		
	for(int i = 0;i<ROW+1;i++)
	   for(int j = 0;j<COL+1;j++)
	   		Onehot[i][j] = 0;
			    
	int col = 0;
	int row = 0;
	list<string>::iterator it1;
	list<string>::iterator it2;
	//¶ÔÃ¿Ò»ÐÐ´Ê»ã×ö´¦Àí£¬¿´ÊÇ·ñ³öÏÖ´Ê»ãÏòÁ¿ÀïµÄµ¥´Ê£¬ÒÔ¼°³öÏÖµÄ´ÎÊý 
	for(it1 = lineword.begin();  it1 != lineword.end(); it1++){
		 string  linew = *it1; 
		 col=0;
		 for(it2 = wordvector.begin();  it2 != wordvector.end(); it2++){
		 	string vec = *it2;
		 	vec =' '+vec+ ' ';
		    int num = fun3(linew,vec);
		 	Onehot[row][col] = num;
		 	Onehot[row][COL] += num;
			if(num)  Onehot[ROW][col]++; 
			col++;
		 }
		 row++;
	} 
	
	
	for(int i=0 ;i <ROW;i++){
		for(int j =0 ;j<COL;j++){
			Onehot[i][j] = double(Onehot[i][j])/double(Onehot[i][COL])*log2(double(ROW)/double(1+Onehot[ROW][j])); 
			//TFIDF<<Onehot[i][j]<<' ';
		}
		//TFIDF<<'\n';
	}
	cout<<"IFIDF finish"<<endl;
	
	int sum = 0;
    fstream  valid("E:\\files\\SMIE\\third_up\\ai\\lab2(KNN+NB)\\DATA\\regression_dataset\\validation_set.csv");
	getline(valid,a);
	vector <double*> validonehot;
	a="";
	while(getline(valid,a)){
		sum++;
		size_t pos = a.find(',');  //ÕÒ³öµÚ¶þ¸öÖÆ±í·ûµÄÎ»ÖÃ 
		stringstream ss1;
		ss1<<a.substr(pos);
		node tmp;
		char ttmp;
		for(int i =0;i<6;i++){
		    ss1>>ttmp;
			ss1>>tmp.mood[i];
		}
		labelvalidation1.push_back(tmp);
	    //ºóÃæÊÇlabel
		int len =  a.substr(pos).size();	
		a = ' '+a.substr(0,a.size()-len)+' '; // ÔÚÄÚÈÝµÄÇ°ºó¼ÓÈë¿Õ¸ñ£¬ÒÔ±ãÃ¿¸öµ¥´ÊµÄ·Ö¸î
		stringstream ss;
		ss<<a;
		string tmp1;
		int wordsum = 0;
		while(ss>>tmp1)  wordsum++;
		double *line = new double[COL];
		int coli = 0;
		for(it2 = wordvector.begin();  it2 != wordvector.end(); it2++){	 	
			 	 string vec = *it2;
			 	 vec =' '+vec+ ' ';
			    int num = fun3(a,vec);
			 	 line[coli] = (double)num/double(wordsum)*log2(double(ROW)/double(1+Onehot[ROW][coli]));
			     coli++;
		}
		validonehot.push_back(line);//µÃµ½Ã¿Ò»ÐÐµÄÖµ 
		//¸úÑµÁ·¼¯Ã¿Ò»ÐÐ×ö±È½Ï 
		a="";
	}
	
	cout<<"train onehot finish"<<endl;
	
	struct node mink;
	struct node minsum;
	
	//minsum.mood[0]= minsum.mood[1] = minsum.mood[2]= minsum.mood[3] = minsum.mood[4]= minsum.mood[5] = 100000;
  cout<<"<validonehot.size():   "<<validonehot.size()<<endl;
		   for(int i = 0;i<validonehot.size();i++){
		   	  	double *line = validonehot[i];
		     	vector <dislabel> Map; 
		     	
				for(int row = 0; row<ROW ; row++){
					double leng = 0;
					double sum1 = 0,sum2=0;
					 for(int col = 0; col<COL ; col++){
					 	sum1 += Onehot[row][col]*Onehot[row][col];
					    sum2 += line[col]*line[col];
						 leng += Onehot[row][col]*line[col];
					 }
					 if(sum2 == 0) sum2=1;
					 leng = leng/(double)sqrt(sum1*sum2);
					 dislabel tmp;
					 tmp.dis = leng;
					 tmp.label = labeltrain1[row];
					 Map.push_back(tmp);//µÃµ½ÓëÃ¿Ò»ÐÐµÄ¾àÀë 
				}
			//µÃµ½ÁËÓëÃ¿Ò»ÐÐÖ®¼äµÄÅÅÃû£»
				sort(Map.begin(),Map.end(),cmp1);
				
				node labelpredict = labelpre1(Map,11);
	
				for(int ij =0;ij<6;ij++)
				  test<<labelpredict.mood[ij]<<',';
				test<<endl;
			//cout<<"labelnow == "<<labelnow <<endl<< " predict :     "<<labelpredict<<endl;
		}

	for(int i = 0; i<ROW; i++)
	   delete [] Onehot[i];
	
	train.close();	
	test.close();
	valid.close();
}
int main(){
	fenlei();
	cout<<"huigui finish"<<endl;
	lineword.clear();
	wordvector.clear();
	huigui();
	return 0;
} 


