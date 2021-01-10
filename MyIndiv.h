#include "MyRect.h"

using namespace std;

class MyIndiv: public std::vector<MyRect>
{
public:
	MyIndiv():m_dFitness(0.) {}
	MyIndiv(unsigned numOfAlleles):m_dFitness(0.), std::vector<MyRect>(numOfAlleles) {}
	MyIndiv(const MyIndiv& i) {
		this->assign(i.begin(), i.end());
		this->m_dFitness = i.m_dFitness;
	}
	MyIndiv& operator =(const MyIndiv& r) {
		this->assign(r.begin(), r.end());
		this->m_dFitness = r.m_dFitness;
		return *this;
	}
	bool operator ==(const MyIndiv& r) {
		if (this->size() != r.size())
		{
			return false;
		}
		else
		{
			for (MyIndiv::size_type i = 0; i < this->size(); i++)
			{
				if (this->at(i) != r.at(i))
				{
					return false;
				}
				else continue;
			}
			return true;
		}
	}
public:
	double m_dFitness;
};