
#include "mdfAlg.h"


CheckInNNAlg(
  Mdf,
  "mdf",
  "Most Distant of the Father Algorithm\n\
  -r <f|s|u>      Pruning rules: f:Fukunaga, s:Sibling, u:Ullman"
)


//-------------------------------------------------------------------
//    Options set up
//-------------------------------------------------------------------

Mdf::Mdf( string data, Oracle *oracle ) 
{
	mOracle = oracle;
	root = 0;
	fRule = false;
	sRule = false;
	uRule = false;

        string token;
        istringstream ss(data);
        while( ss >> token ) {
		if( token == "-r" ) 
		{
			if( !( ss >> token ) ) 
			{
				cerr << "ERROR in Mdf: I cant read the rules" << endl;
				exit(-1);
			}
			for( unsigned i = 0; i < token.length(); i++ )
			{
				if( token[i] == 'f' )
				{
					fRule = true;
				}
				else if( token[i] == 's' )
				{
					sRule = true;
				}
				else if( token[i] == 'u' )
				{
					uRule = true;
				}
				else {
	                                cerr << "ERROR in Mdf: unknown rule (" << token[i] <<")" << endl;
	                                exit(-1);
	                        }
			}
		} 
		else 
		{
			cerr << "ERROR in Mdf: unknown option (" << token <<")" << endl;
 			exit(-1);
		}
 	
 	} 

}


//---------------------------------------
//
Mdf::~Mdf() 
{
	delete root;
}



//-------------------------------------------------------------------
// Insert Bulk
//-------------------------------------------------------------------
void Mdf::InsertBulk( Point _db[], int size ) 
{
	assert( size >= 0 );
	
	if( root != 0 )
		delete root;

	vector<Point> db( _db+1, _db + size );
	vector<double> dLRep( db.size() );

	int lRep = _db[0];

	for( unsigned int i = 0; i < db.size(); i++ ) 
	{
		dLRep[i] = mOracle->GetDistance( lRep, db[i] );
	}

	root = BuildTree( lRep, db, dLRep, 0.0 );

	// print( root );
}

//-------------------------------------

Mdf::Tree *Mdf::BuildTree(  Point lRep, 
                            vector<Point> db,
		            vector<double> dLRep,
                            double minRadius
                          ){

  //
  // ---- Base Case ----
  //
  if( db.size() == 0 ) {
    Tree *t = new Tree;
    if( t == 0 ) {
      cerr << "ERROR in Mdf::buildTree(..)" << endl;
      cerr << "  memory exhausted" << endl;
      exit(-1);
    }
    t->rep = lRep; 
    t->radius = 0; // not needed, done in the default constructor
    t->minRadius = minRadius; // it is done in the father
    t->lChild = 0; // not needed, done in the default constructor
    t->rChild = 0; // not needed, done in the default constructor

    return t;
  }
  // 
  // ---- Inductive Step ----
  //
  //  Computing Right Representant and node radius
  //

//  cout << "p.size(): " << p.size() << endl;
  double radius = -1.0;
  int  pRRep = -1;
  for( unsigned int i = 0; i < db.size(); i++ ) {
    if( dLRep[i] > radius ) {
      radius = dLRep[i];
      pRRep = i;
    }
  }

  Point rRep = db[pRRep];
  db.erase(db.begin() + pRRep );
  dLRep.erase(dLRep.begin() + pRRep );

  //
  // Computing Distances to Right Representant
  //

  vector<double> dRRep(db.size());
  for( unsigned int i = 0; i < db.size(); i++ ) {
    dRRep[i] = mOracle->GetDistance(rRep,db[i]);
  } 

  // distribute the points by its NN

  vector<Point> lP;   // elements of p that are going to go to the left
  vector<double> dLP;
  vector<Point> rP;   // elements of p that are going to go to the right
  vector<double> dRP;
  
  double minRadiusL = radius;
  double minRadiusR = radius;

  for( unsigned int i = 0; i < db.size(); i++ ) 
  {
    if( dLRep[i] < dRRep[i] ) {
      lP.push_back(db[i]);
      dLP.push_back(dLRep[i]);
      if( dRRep[i] < minRadiusR )
        minRadiusR = dRRep[i];
    } else {
      rP.push_back(db[i]);
      dRP.push_back(dRRep[i]);
      if( dLRep[i] < minRadiusL )
        minRadiusL = dLRep[i];
    }
  }

  // recursive call

  Tree *lTree = BuildTree( lRep, lP, dLP, minRadiusL );
  Tree *rTree = BuildTree( rRep, rP, dRP, minRadiusR );
      
  Tree *t = new Tree;
  if( t == 0 ) {
    cout << "ERROR in Mdf::buildTree(..)\n" << endl;
    cout << "  memory exhausted" << endl;
    exit(-1);
  }
  t->rep = lRep;
  t->radius = radius;
  t->lChild = lTree;
  t->rChild = rTree;
  t->minRadius = minRadius;

  return t;
}
    
//-------------------------------------------------------------------
// Insert Incrementally
//-------------------------------------------------------------------

void Mdf::Insert( Point p ) 
{
	assert( p >= 0 );
	
	if( root == 0 ) { // only used when inserting the first point
		vector<Point> db;
		vector<double> dP;
		root = BuildTree( p, db, dP, 0.0 ); 
		return;
	}

	double disToLRep = mOracle->GetDistance(root->rep, p);
  
	Insert(p, root, disToLRep);
  
	// cout << "-- Printing tree --" << endl;
	// print( root );
}


//----------------------
//
void Mdf::Insert( Point p, Mdf::Tree* &t, double disToLRep ) 
{  
  Point lRep = t->rep;
  if( disToLRep > t->radius ) {
    vector<Point> db;
    vector<double> dLRep;

    ExtractPoints( t, db );
    
    for( unsigned int i = 0; i < db.size(); i++ ) {
      dLRep.push_back( mOracle->GetDistance( lRep, db[i]));
    }
    
    db.push_back(p);
    dLRep.push_back(disToLRep);
    double minRad = t->minRadius;
    delete t;

    t = BuildTree( lRep, db, dLRep, minRad ); // CAMBIALO
    return;
  } 

  if( t->lChild == 0 ) { 
    if( t->rChild != 0 ) {
      cout << "ERROR in Mdf::insert(..)" << endl;
      cout << "  this shouldn't happend" << endl;
      exit(-1);
    }
    vector<Point> db;
    vector<double> dP;

    db.push_back(p);
    dP.push_back(disToLRep);
    double minRad = disToLRep;
    delete t;
    
    t = BuildTree(lRep, db, dP, minRad); 
    
    return;
  }

  double disToRRep = mOracle->GetDistance( p, t->rChild->rep );
  
  if( disToLRep < disToRRep ) 
  {
    Insert( p, t->lChild, disToLRep );
    if( disToRRep < t->rChild->minRadius )
      t->rChild->minRadius = disToRRep;
  } else {
    Insert( p, t->rChild, disToRRep );
    if( disToLRep < t->lChild->minRadius )
      t->lChild->minRadius = disToLRep;
  }
}


//-------------------
//
void Mdf::ExtractPoints( Mdf::Tree* t, vector<Point> &db) 
{
  if( t->lChild != 0 ) {
    ExtractPoints(t->lChild, db);
  }
  if( t->rChild != 0 ) {
    db.push_back(t->rChild->rep);
    ExtractPoints(t->rChild, db);
  }
}


//-------------------------------------------------------------------
//    Search
//-------------------------------------------------------------------

void Mdf::SearchNN( const Mdf::Tree  *t,
	      double disToLeftRep ) {

  if( t->lChild == 0 && t->rChild == 0 ) {  // if I'm in a leave
    return;
  }

  // pruning what can be pruned without computing any new distance
  // if rules s, g or t are activated and right node can be pruned
  if( sRule && disToLeftRep + mNNDistance < t->lChild->minRadius ) {
        
      // rule f not activated or left node cannot be pruned
    if( !fRule || disToLeftRep - mNNDistance < t->lChild->radius ){
        // looking only on the left --> sibling node has been pruned 
        SearchNN( t->lChild, disToLeftRep);
    }
    return; // both were pruned.
  }    

  double disToRightRep = mOracle->GetDistance( qp, t->rChild->rep);
  if( disToRightRep < mNNDistance ){
    mNNDistance = disToRightRep;
    mNNPoint = t->rChild->rep;
  }
  
  if( disToLeftRep < disToRightRep ) {
    // pruning and seaching if needed LEFT
    if(    (!fRule || disToLeftRep - mNNDistance < t->lChild->radius ) 
        && (!sRule || disToRightRep + mNNDistance > t->rChild->minRadius )
        && (!uRule || disToLeftRep - mNNDistance < disToRightRep + mNNDistance )
      ) {
        SearchNN( t->lChild, disToLeftRep);
    }
    // pruning and seaching if needed RIGHT
    if(    (!fRule || disToRightRep - mNNDistance < t->rChild->radius ) 
        && (!sRule || disToLeftRep + mNNDistance > t->lChild->minRadius ) 
        && (!uRule || disToLeftRep + mNNDistance > disToRightRep - mNNDistance )
      ) {
      SearchNN( t->rChild, disToRightRep );
    }
  } else {
    // pruning and seaching if needed RIGHT
    if(    (!fRule || disToRightRep - mNNDistance < t->rChild->radius ) 
        && (!sRule || disToLeftRep + mNNDistance > t->lChild->minRadius ) 
        && (!uRule || disToLeftRep + mNNDistance > disToRightRep - mNNDistance )
      ) {
      SearchNN( t->rChild, disToRightRep );
    }
    // pruning and seaching if needed LEFT
    if(    (!fRule || disToLeftRep - mNNDistance < t->lChild->radius )
        && (!sRule || disToRightRep + mNNDistance > t->rChild->minRadius ) 
        && (!uRule || disToLeftRep - mNNDistance < disToRightRep + mNNDistance )
      ) {
      SearchNN( t->lChild, disToLeftRep );
    }
  }
}

//--------------------------------------
void Mdf::SearchNN( Point _qp ) 
{
	assert( _qp >= 0 );
	qp = _qp;
	mNNPoint = root->rep;
	mNNDistance = mOracle->GetDistance( qp, root->rep );

	return SearchNN( root, mNNDistance );
}



//-------------------------------------------------------------------
//  Auxiliary
//-------------------------------------------------------------------

void Mdf::Print( Mdf::Tree *t ) 
{
  if( t != 0 ) {
    cout << "rep:" << t->rep << endl;
    cout << "radius: " << t->radius << endl;
    cout << "minRadius: " << t->minRadius << endl;
    cout << endl;
    Print( t->lChild );
    Print( t->rChild );
  }
}
