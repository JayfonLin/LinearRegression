#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>
using namespace std;
double alpha = 0.1;
double lamda = 1000;
double qf;
double t[385];
double* arr_eh;
int m;
struct Rec{
	int id;
	double prediction, x[385];
};
double h(Rec rec){
	double result = 0.0;
	for (int i = 0; i < 385; i++){
		result += (t[i]*rec.x[i]);
	}
	return result;
}
void precal(Rec* recs){
	for (int i = 0; i < m; i++){
		arr_eh[i] = h(recs[i])-recs[i].prediction;
	}
	qf = 1.0-alpha*lamda/m;
}

double sh(Rec* recs, int tt){
	double result = 0.0;
	for (int i = 0; i < m; i++){
		result += (arr_eh[i]*recs[i].x[tt]);
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
	for (int i = 0; i < 385; i++){
		result += thetas[i]*rec.x[i];
	}
	return result;
}
double cost(Rec* recs, double* thetas){
	double result = 0.0;
	double temp;
	for (int i = 0; i < m; i++){
		temp = th(recs[i], thetas)-recs[i].prediction;
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
	buffer = "RELR";
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
	fstream of(coststring.c_str(), ios::out);
	fstream testf("test.csv", ios::in);
	of << "pass," << "total cost\n";

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
	double precost = -1;
	bool preFirst = true;
	for (int i = 0; i < 10000; i++){
		printf("pass %d\n", i);
		of << i << ",";
		precal(recs);
		double temp[385];
		for (int j = 0; j < 385; j++){
			temp[j] = gd(recs, j); 
		}
		
		double cosv = cost(recs, temp);
		/*if (precost < cosv || precost - cosv < 0.000001){
			if (preFirst){
				preFirst = false;

			}else {
				alpha *= 0.8;
				i--;
				printf("alpha multiply 0.8: %lf\n", alpha);
				continue;
			}
		}*/
		for (int j = 0; j < 385; j++){
			t[j] = temp[j];
		}
		cout << fixed << fixed << setiosflags(ios::showpoint) << setprecision(6) << "total cost: " << cosv << endl;
		of << cosv << "\n";
		precost = cosv;
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
	of.close();
	return 0;
}