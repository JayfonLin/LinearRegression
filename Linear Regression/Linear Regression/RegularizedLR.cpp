#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
using namespace std;
const int mypower = 4;
const int t_size  = 384*mypower+1;
double alpha = 0.01;
double lamda = 10;
double qf;
double t[t_size];
double* arr_eh;
int m;
struct Rec{
	int id;
	double prediction, x[385];
};
double h(Rec rec){
	double result = 0.0;
	int i;
	/*
	for (i = 0; i < 385; i++){
		result += (t[i]*rec.x[i]);
	}
	for (int j = 1; j < 385; j++,i++){
		result += (t[i]*rec.x[j]*rec.x[j]);
	}
	*/
	double temp;
	for (i = 0; i < t_size; i++){
		if (i < 385){
			result += (t[i]*rec.x[i]);
		}else{
			int k = i%384;
			int p = i/384;
			if (k == 0){
				p--;
				k = 384;
			}
			
			temp = rec.x[k];
			for (int j = 0; j < p; j++){
				temp *= rec.x[k];
			}
			result += t[i]*temp;
		}
		
	}
	return result;
}
void precal(Rec* recs){
	for (int i = 0; i < m; i++){
		arr_eh[i] = h(recs[i])-recs[i].prediction;
	}
	qf = 1.0-alpha*lamda/m;
	//qf = 1;
}

double sh(Rec* recs, int tt){
	double result = 0.0;
	double temp;
	int p;
	int k;
	if (tt < 385){
		p = tt/385;
		k = tt;
	}
	else{
		p = tt/384;
		k = tt%384;
		if (k == 0){
			p--;
			k = 384;
		}
	}
	for (int i = 0; i < m; i++){
		temp = recs[i].x[k];
		for (int j = 0; j < p; j++){
			temp *= recs[i].x[k];
		}
		
		result += (arr_eh[i]*temp);
	}
	return result;
}
double gd(Rec* recs, int tt){
	if (0 == tt){
		return t[tt] - alpha*sh(recs, tt)/m;
	}
	return t[tt]*qf - alpha*sh(recs, tt)/m;
}
double th(Rec rec, double* thetas){

	double result = 0.0;
	int i;
	double temp;
	for (i = 0; i < t_size; i++){
		if (i < 385){
			result += (thetas[i]*rec.x[i]);
		}else{
			int k = i%384;
			int p = i/384;
			if (k == 0){
				p--;
				k = 384;
			}
			
			temp = rec.x[k];
			for (int j = 0; j < p; j++){
				temp *= rec.x[k];
			}
			result += t[i]*temp;
		}
		
	}
	return result;
}

double cost(Rec* recs, double* thetas){
	double result = 0.0;
	double temp;
	for (int i = 0; i < m; i++){
		temp = th(recs[i], thetas)-recs[i].prediction;
		//temp = arr_eh[i];
		result += (temp * temp);
	}
	/*double ts = 0.0;
	for (int i = 1; i < 385; i++){
		ts += (thetas[i]*thetas[i]);
	}
	result += (lamda*ts);*/
	return result /= (2.0*(double)m);
}
void norm(Rec* recs){
	double max[385], min[385];
	
	for (int i = 1; i < 385; i++){
		double sum = 0.0;
		bool isFirst = true;
		for (int j = 0; j < m; j++){

			sum += recs[j].x[i];
			if (isFirst){
				isFirst = false;
				max[i] = recs[j].x[i];
				min[i] = recs[j].x[i];
				continue;
			}
			if (max[i] < recs[j].x[i]){
				max[i] = recs[j].x[i];
			}
			if (min[i] > recs[j].x[i]){
				min[i] = recs[j].x[i];
			}
		}
		double ave = sum/(double)m;
		if (max[i] - min[i] < 0.000001)
			continue;
		for (int j = 0; j < m; j++){
			recs[j].x[i] = (recs[j].x[i] - ave)/(max[i]-min[i]);
		}
	}
}


void test_input(Rec* recs){
	
	for (int i = 0; i < m; i++){
		cout << "id: " << recs[i].id << ",prediction: " << recs[i].prediction;
		for (int j = 0; j < 385; j++){
			cout << "," << recs[i].x[j];
		}
		printf("\n");
		system("pause");
	}
}


int main()
{
	int decimal, sign;
	string buffer;
	buffer = "RELR5";
	/*buffer = fcvt(alpha, 2, &decimal, &sign);
	if (decimal <= 0){
		
		for (int i = 0; i < -decimal; i++){
			buffer = "0"+buffer;
		}
		buffer = "0."+buffer;
	}*/
	string coststring = "cost_"+buffer+".csv";
	string commitstring = "commit_"+buffer+".csv";
	Rec* recs = new Rec[10000];
	fstream f("train.csv", ios::in);
	//fstream of(coststring.c_str(), ios::out);
	fstream testf("test.csv", ios::in);
	//of << "pass," << "total cost\n";

	bool isFirst = true;
	memset(t, 0, sizeof(t));
	
	char c;
	int count = -1;
	cout << "open file finished\n";
	while (!f.eof()){
		string temp = "";
		if (isFirst){
			isFirst = false;
			
			getline(f, temp);
			cout << "first pass finished\n";
			continue;
		}
		count++;
		if (count >= 10000){
			break;
		}
		f >> recs[count].id >> c >> recs[count].prediction;
		recs[count].x[0] = 1;
		for (int i = 1; i < 385; i++){
			f >> c >> recs[count].x[i];
			
		}
		getline(f, temp);
		
	}
	cout << "read file finished\n";
	m = count;
	//norm(recs);
	arr_eh = new double[m];
	
	
	for (int i = 0; i < 10000; i++){
		printf("pass %d\n", i);
		//of << i << ",";
		precal(recs);
		double temp[t_size];
		for (int j = 0; j < t_size; j++){
			temp[j] = gd(recs, j); 
		}
		for (int j = 0; j < t_size; j++){
			t[j] = temp[j];
		}
		
		if (i%20 == 0){
			double cosv = cost(recs, temp);
			cout << fixed << setiosflags(ios::showpoint) << setprecision(6) << "total cost: " << cosv << endl;
		}
		//of << cosv << "\n";
	}
	Rec* trs = new Rec[10000];
	int tcount = -1;
	bool tisFirst = true;
	while (!testf.eof()){
		string temp = "";
		if (tisFirst){
			tisFirst = false;
			
			getline(testf, temp);
			cout << "t first pass finished\n";
			continue;
		}
		tcount++;
		if (tcount >= 10000){
			break;
		}
		testf >> trs[tcount].id;
		trs[tcount].x[0] = 1;
		for (int i = 1; i < 385; i++){
			testf >> c >> trs[tcount].x[i];
		}
		getline(testf, temp);
	}
	double cosv = cost(recs, t);
	cout << "total cost: " << cosv << endl;
	cout <<  "load test file finished\n";
	fstream cf(commitstring.c_str(), ios::out);
	cf << "id,reference\n";
	for (int i = 0; i < tcount; i++){
		trs[i].prediction = h(trs[i]);
		cf << trs[i].id << "," << fixed << setiosflags(ios::showpoint) << setprecision(1) << trs[i].prediction << endl;
	}
	cf.close();
	testf.close();
	f.close();
	//of.close();
	return 0;
}