
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <assert.h>
//#include "oracle.h"
//#include "nnAlg.h"
//#include "checkInNNAlg.h"
#include "checkInOracle.h"

using namespace std;


struct Operation {
  Operation( string _name, int _size ) {
    name = _name;
    size = _size;
    print = true;
    heading = false;
    repetitions = 1;
  }

  string name;
  int    size;
  bool   print;
  bool   heading;
  int    repetitions;
};
  





//-------------------------------------------------------------
//
void PrintUsage(string mainFileName)
{
	cout << "* Workbench for Nearest Neighbour algorithms." << endl;
	cout << "* Usage:  " << mainFileName << " [options]" << endl;
	cout << "  -b <size> [options]     Bulk initialization" << endl;
	cout << "  -i <size> [options]     Incremental initialization" << endl;
	cout << "  -t <size> [options]     Test sample size" << endl;
	cout << "       -np                No print" << endl;
	cout << "       -nh                No heading" << endl;
	cout << "       -ph                Print heading" << endl;	
	cout << "       -r <int>           Repetitions" << endl;
	cout << "  -s <int>                Seed" << endl;
	cout << "  -check                  Compare with the brute force" << endl;
	cout << "  -debug                  Debug mode" << endl;
	cout << "  -h                      Help" << endl;
	cout << "  -alg <name> \"<options>\" NN algorithm" << endl; 
	cout << "  -ora <name> \"<options>\" Data generator" << endl;
	cout << endl;

	cout << "--- NN Algorithm options ---" << endl;
//	CheckInNNAlg::listInfo();

	cout << "--- Oracle options ---" << endl;
	CheckInOracle::listInfo();
}


//-------------------------------------------------------------
//
bool checkInputParameters( int argc, char* argv[], 
				int &seed, bool &check, bool &debug,
				string &nnAlg, string &nnAlgOpts, 
				string &oracle, string &oracleOpts, 
				vector<Operation> &operationList)
{
	bool selectedOneMode = false;
	
	for( int i = 1; i < argc; i++ ) 
	{
		if( string( argv[i] ) == "-b" ) 
		{
			if(i < argc-1 && argv[i+1][0] != '-')
			{
				operationList.push_back( Operation(string("-b"), atoi(argv[++i]) ) );
				selectedOneMode = true;
			}
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <size>" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-i" ) 
		{
			if(i < argc-1 && argv[i+1][0] != '-')
			{
				operationList.push_back( Operation(string("-i"), atoi(argv[++i]) ) );
				selectedOneMode = true;
			}
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <size>" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-t" ) 
		{
			if(i < argc-1 && argv[i+1][0] != '-')
			{
				operationList.push_back( Operation(string("-t"), atoi(argv[++i]) ) );
				selectedOneMode = true;
			}
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <size>" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-np" ) 
		{
			operationList[operationList.size()-1].print = false;
		} 
		else if( string( argv[i] ) == "-nh" ) 
		{
			operationList[operationList.size()-1].heading = false;
		} 
		else if( string( argv[i] ) == "-ph" ) 
		{
			operationList[operationList.size()-1].heading = true;
		} 
		else if( string( argv[i] ) == "-r" ) 
		{
			if(i < argc-1 && argv[i+1][0] != '-')
				operationList[operationList.size()-1].repetitions = atoi(argv[++i]);
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <repetitions>" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-s" ) 
		{
			if(i < argc-1 && argv[i+1][0] != '-')
				seed = atoi(argv[++i]);
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <seed>" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-check" ) 
		{
			check = true;
		} 
		else if( string( argv[i] ) == "-debug" ) 
		{
			debug = true;
		} 
		else if( string( argv[i] ) == "-alg" ) 
		{
			if(i < argc-2 && argv[i+1][0] != '-' && argv[i+2][0] != '-')
			{
				nnAlg = argv[++i];
				nnAlgOpts = argv[++i];
			}
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <NN algorithm> \"<options>\"" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-ora" ) 
		{
			if(i < argc-2 && argv[i+1][0] != '-' && argv[i+2][0] != '-')
			{
				oracle = argv[++i];
				oracleOpts = argv[++i];
			}
			else {
				cout << "ERROR: Option '" << argv[i] << "' requires: <Data generator> \"<options>\"" << endl;
				exit(-1);
			}
		} 
		else if( string( argv[i] ) == "-h" )
		{
			PrintUsage( argv[0] );
			exit(0);
		} 
		else 
		{
			cout << "ERROR: unknown option '" << argv[i] << "'" << endl;
			exit(-1);
		}
	}

	
	if( selectedOneMode == false) {
		cout << "ERROR: You have to select at least one mode: -b|-i|-t" << endl;
		exit(-1);
	}			
	
	/*
	if( testSize == 0 ) {
		cout << "ERROR: you have to specify a test size (\"-t\" option) " << endl;
		exit(-1);
	}
	*/

	if( oracle.empty() ) {
		cout << "ERROR: you have to specify an oracle (\"-ora\" option) " << endl;
		exit(-1);
	}

	if( nnAlg.empty() ) {
		cout << "ERROR: you have to specify a NN algorithm (\"-alg\" option) " << endl;
		exit(-1);
	}

}


//-------------------------------------------------------------
//
void PrintOperation(bool heading, string operation, int size, int seed, float dist, 
					string alg, string algOpts, string ora, string oraOpts)
{
	if( heading ) {
		printf("%6s %6s %4s %7s %5s %10s %6s %10s\n",
				"# Oper", "size", "Seed", "Dist", 
				"Alg", "Alg_Opts", "Ora", "Ora_Opts");
	}

	printf("%6s %6d %4d ", operation.c_str(), size, seed);
	
	if(operation == "-t")
		printf("%7.2f ", dist);
	else
		printf("%7d ", (int)dist);
	
	printf("%5s %-10s %6s %-10s\n",			
			alg.c_str(), algOpts.c_str(), ora.c_str(), oraOpts.c_str() );
}



//-------------------------------------------------------------
// Inserting BULK
void OperationInsertingBulk(Operation operation, int seed, 
		string nnAlg, string nnAlgOpts, string oracle, string oracleOpts)
{
	int accDis = 0;
	int bulk = operation.size;	
/*	Point v[bulk];
	
	for( int i = 0; i < bulk; i++ ) 
	{
		v[i] = o->newPoint();
	}
	int dis = o->distanceComputations();
	s->insertBulk(v,bulk);  // this can only be done once 
	dis = o->distanceComputations() - dis;
	accDis += dis;

	if( check ) {
		bf->insertBulk(v,bulk);
	}
	*/
	
	if( operation.print )
		PrintOperation(operation.heading, "-b", bulk, seed, accDis,  
					nnAlg, nnAlgOpts, oracle, oracleOpts);
}


//-------------------------------------------------------------
// Inserting One by One
void OperationInsertingOneByOne(Operation operation, int seed, 
		string nnAlg, string nnAlgOpts, string oracle, string oracleOpts)
{
	int accDis = 0;
	int inc = operation.size;
	
	/*for( int i = 0; i < inc; i++ ) {
		Point p = o->newPoint();
		//    cout << "--- i: " << i << endl;
		int dis = o->distanceComputations();
		s->insert(p);
		dis = o->distanceComputations() - dis;
		accDis += dis;
		if( check ) {
			bf->insert(p);
		}
	}
	*/
	
	if( operation.print )
		PrintOperation(operation.heading, "-i", inc, seed, accDis, 
				nnAlg, nnAlgOpts, oracle, oracleOpts);

}



//-------------------------------------------------------------
// Testing
void OperationTesting(Operation operation, int seed, 
		string nnAlg, string nnAlgOpts, string oracle, string oracleOpts)
{
	int accDis = 0;
	int testSize = operation.size;
	
	/*for( int i = 0; i < testSize; i++ ) 
	{
		Point p = o->newPoint();
		int dis = o->distanceComputations();
		s->NN(p);
		dis = o->distanceComputations() - dis;
		accDis += dis;
		if(check) {
			bf->NN(p);
			if( s->nnDistance() != bf->nnDistance() ) {
				cout << "ERROR: the result does not agrees with the brute force" << endl;
				cout << "iteration: " << i << endl;
				cout << "bf point: " << bf->nnPoint() << " dis: " << bf->nnDistance() << endl;
				cout << "alg point: " << s->nnPoint() << " dis: " << s->nnDistance() << endl;
				exit(-1);
			}
		}
	} 
	*/
	
	if( operation.print )
		PrintOperation(operation.heading, "-t", testSize, seed, accDis/double(testSize), 
				nnAlg, nnAlgOpts, oracle, oracleOpts);
}



//-------------------------------------------------------------
//
int main( int argc, char* argv[] ) 
{
	int seed = 1;
	bool check = false;
	bool debug = false;
	string nnAlg, nnAlgOpts;
	string oracle, oracleOpts;
	vector<Operation> operationList;


	checkInputParameters( argc, argv, 
						  seed, check, debug, 
						  nnAlg, nnAlgOpts, oracle, oracleOpts,
						  operationList);


	if(debug)
	{
		cout << "# Seed : " << seed << endl;
		cout << "# check: " << (check ? "true" : "false") << endl;
		cout << "# Alg  : " << nnAlg << " \"" << nnAlgOpts << "\"" << endl;
		cout << "# Ora  : " << oracle << " \"" << oracleOpts << "\"" << endl;
	}

	srand(seed);

/*
  	Oracle* o = CheckInOracle::object( oracle, oracleOpts);
	if( o == 0 ) {
    	cout << "ERROR: unknown oracle '" << oracle << "'" << endl;
    	exit(-1);
 	}

  	NNAlg* s = CheckInNNAlg::object(nnAlg, o, nnAlgOpts );
  	if( s == 0 ) {
    	cout << "ERROR: unknown NN algorithm '" << nnAlg << "'" << endl;
    	exit(-1);
  	}

  	NNAlg* bf = 0;
  	if( check ) {
    	bf = CheckInNNAlg::object("bf", o, "");
  	}
*/

	// MAIN LOOP
	for( unsigned int opn = 0; opn < operationList.size(); opn++ ) 
	{
		for( int r = 0; r < operationList[opn].repetitions; r++ ) 
		{
			if( operationList[opn].name == "-b" ) 
			{
				// Inserting BULK
				OperationInsertingBulk(operationList[opn], seed, 
							nnAlg, nnAlgOpts, oracle, oracleOpts);
			} 
			
			else if( operationList[opn].name == "-i" ) 
			{
				// Inserting One by One
				OperationInsertingOneByOne(operationList[opn], seed, 
							nnAlg, nnAlgOpts, oracle, oracleOpts);
			} 
			
			else if( operationList[opn].name == "-t" ) 
			{
				// Testing
				OperationTesting(operationList[opn], seed, 
							nnAlg, nnAlgOpts, oracle, oracleOpts);
			} 
			else 
			{
				cout << "ERROR: this shouldent happed" << endl;
				cout << "unknown option: '" << operationList[opn].name << "'" << endl;
			}
		}
	}

	return 0;
}


