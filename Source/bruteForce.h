#ifndef bruteForce_h
#define bruteForce_h

#include <vector>
#include "nnAlg.h"
#include "checkInNNAlg.h"


class BruteForce: public NNAlg 
{
	public:

		BruteForce( vector<string> data, Oracle * oracle );

		void Insert( Point);

		void InsertBulk( Point p[], int c);

		void SearchNN( Point p );
		

	private:
		
		vector<Point> db;

};

#endif