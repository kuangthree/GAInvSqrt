#include<iostream>
#include<cstdlib>
#include<ctime>
#include<climits>
#include<algorithm>
#include<vector>
using namespace std;
const double Pc = 0.5;
const double Pm = 0.1;
const int    M  = 100;
const int    G  = 10000000;
const double Tf = 0.999;

const double UNCOMPUTED = -1.0;
struct Chr{
	int value;
	double fitness;
	Chr(int v):value(v),fitness(UNCOMPUTED){}
	Chr():value(0),fitness(UNCOMPUTED){}
	bool operator< (const Chr& m) {
		if (fitness > m.fitness)return true;
		if (fitness == m.fitness && value > m.value)return true;
		return false;
	}
};
float InvSqrt(float x,int magic_const)
{
	float xHalf = x * 0.5f;
	int i = *(int*)&x;
	i = magic_const - (i >> 1);
	x = *(float*)&i;
	x = x*(1.5f - xHalf * x * x);
	return x;
}
bool contains(const vector<Chr>& vector, int value) {
	for (Chr c : vector) {
		if (c.value == value) {
			return true;
		}
	}
	return false;
}
void printVector(const vector<Chr>& pop) {
	cout << hex;
	for (auto a : pop) {
		cout << "0x" << a.value << " ";
	}
	cout << dec << endl;
}
void printVectorWithFitness(const vector<Chr>& pop) {
	for (auto a : pop) {
		cout << hex;
		cout << "0x" << a.value << " F:";
		cout << dec << a.fitness << "\t";
	}
	cout << endl;
}
void computeFitness(Chr& chr)
{
	if (chr.fitness < 0) {
		double max = 0;
		for (float i = 0.1f; i < 500.0f; i+=0.5f) {
			float a = InvSqrt(i, chr.value);
			float b = 1.0f / sqrt(i);
			float m = abs(a - b);
			float rm = m / b;
			if (rm > 1) {
				chr.fitness = 0;
				return;
			}
			if (rm > max) {
				max = rm;
			}
		}
		chr.fitness = 1 - max;
	}
}
void computeFitness(vector<Chr>& pop) {
	for (Chr& c : pop) {
		computeFitness(c);
	}
}
void select(const vector<Chr>& pop, Chr&a, Chr&b) {
	double p = rand() / (double)RAND_MAX;
	double sump = 0;
	for (Chr c : pop) {
		sump += c.fitness;
	}
	double sum = 0;
	for (int i = 0; i < pop.size(); i++) {
		sum += pop[i].fitness / sump;
		if (p <= sum) {
			a = pop[i];
			break;
		}
	}
	p = rand() / (double)RAND_MAX;
	sum = 0;
	for (int i = 0; i < pop.size(); i++) {
		sum += pop[i].fitness / sump;
		if (p <= sum) {
			b = pop[i];
			break;
		}
	}
}
int getMaxStep(int g)
{
	return 0xFFFFFFF / (g/100+1);
}
int main()
{
	cout << "Start Genetic Algorithm." << endl;
	srand(time(0));
	vector<Chr> pop;
	int max_step = 0xFFFFFFF;
	do {
		Chr chr = (int) ( (rand() / (double)RAND_MAX) * 0xFFFFFFFF );
		if (!contains(pop, chr.value)) {
			pop.push_back(chr);
		}
	} while ( pop.size() < M );
	cout << "Generate first generation :" << endl;
	printVector(pop);
	int g = 0;
	double tf = 0.0;
	do {
		vector<Chr> newPop;
		do {
			computeFitness(pop);
			sort(pop.begin(), pop.end());
			Chr a, b;
			select(pop, a, b);
			if (rand() / (double)RAND_MAX < Pc) {
				long long c = (long long)a.value + (long long)b.value;
				c = c / 2;
				a = Chr((int)c);
			}
			if (rand() / (double)RAND_MAX < Pm * 2) {
				if (rand() % 2) {
					a.value = a.value - (int)(max_step*(rand()/(double)RAND_MAX));
				}
				else {
					a.value = a.value + (int)(max_step * (rand() / (double)RAND_MAX));
				}
				a.fitness = UNCOMPUTED;
			}
			if (rand() / (double)RAND_MAX < Pm * 2) {
				if (rand() % 2) {
					b.value = b.value - (int)(max_step * (rand() / (double)RAND_MAX));
				}
				else {
					b.value = b.value + (int)(max_step * (rand() / (double)RAND_MAX));
				}
				b.fitness = UNCOMPUTED;
			}
			computeFitness(a);
			computeFitness(b);
			if (a.fitness > 0 && !contains(newPop,a.value)) {
				newPop.push_back(a);
			}
			if (b.fitness > 0 && !contains(newPop, b.value)) {
				newPop.push_back(b);
			}
		} while (newPop.size() < M);
		for (int i = 0; i < M /5 ; i++) {
			if (!contains(newPop, pop[i].value)) {
				newPop.push_back(pop[i]);
			}
		}
		pop = newPop;
		sort(pop.begin(), pop.end());
		while (pop.size() > M) {
			pop.pop_back();
		}
		g++;
		if (g % 100 == 0) {
			cout << "Generate " << g << ": " << endl;
			printVectorWithFitness(pop);
		}
		tf = pop[0].fitness;
		max_step = getMaxStep(g);
	} while (g <= G && tf < Tf);
	system("pause");
	return 0;
}