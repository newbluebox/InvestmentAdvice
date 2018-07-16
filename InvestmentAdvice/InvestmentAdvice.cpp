// InvestmentAdvice.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// C++11
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen( 42 ); //Standard mersenne_twister_engine seeded with rd()

class DataGenerator
{
public:
    DataGenerator( int seed ) :
        m_gen( seed ), m_dis_1( 0, 1 ), m_money( 1000000 )
    {
        std::uniform_int_distribution<> dis_experts( 10, 50 - 1 );
        m_numExperts = dis_experts( m_gen );

        for ( int i = 0; i < m_numExperts; ++i )
        {
            m_stDev.push_back( m_dis_1( m_gen ) * 20 );
            m_accuracy.push_back( m_dis_1( m_gen ) );
            m_actual.push_back( 0 );
            m_reported.push_back( 0 );
            m_result.push_back( 0 );
        }
    };

    void generateNextDay()
    {
        std::normal_distribution<> dis_normal( 0.0, 1.0 );

        for ( int i = 0; i < m_numExperts; ++i )
        {
            m_actual[ i ] = std::min( 1.0, std::max( -1.0, dis_normal( m_gen ) * 0.1 ) );

            double u = m_dis_1( m_gen );
            if ( u < m_accuracy[ i ] )
                m_reported[ i ] = (int)std::round( std::min( 100.0, std::max( -100.0, dis_normal( m_gen ) * m_stDev[ i ] + m_actual[ i ] * 100 ) ) );
            else
                m_reported[ i ] = (int)std::round( 100 * std::min( 1.0, std::max( -1.0, dis_normal( m_gen ) * 0.1 ) ) );
        }
    }

    void applyResults( const vector< int > & invest )
    {
        for ( int i = 0; i < m_numExperts; ++i )
        {
            m_result[ i ] = (int)std::floor( invest[ i ] * m_actual[ i ] );
            m_money += m_result[ i ];
        }
    }


private:
    mt19937 m_gen;
    int m_numExperts;
    std::uniform_real_distribution<> m_dis_1;
    vector< double > m_stDev;
    vector< double > m_accuracy;
    vector< double > m_actual;

public:
    int m_money;
    vector< int > m_reported;
    vector< int > m_result;
};


class InvestmentAdvice
{
public:
    vector<int> getInvestments( vector<int> advice, vector<int> recent, int money, int timeLeft, int roundsLeft )
    {
        vector<int> ret( advice.size() );
        for ( int i = 0; i < advice.size(); i++ )
        {
                ret[ i ] = 100;
        }

        return ret;
    }
};

// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector( vector<T>& v ) {
    for ( int i = 0; i < v.size(); ++i )
        cin >> v[ i ];
}

int main() {
    InvestmentAdvice sol;
    DataGenerator dataGen( 33 );

    double test = 0;

    int days = 10;

    for ( int i = 0; i < days; ++i )
    {
        dataGen.generateNextDay();
        vector< int > invest = sol.getInvestments( dataGen.m_reported, dataGen.m_result, dataGen.m_money, 0, i );
        dataGen.applyResults( invest );

        test += dataGen.m_result[ 4 ] * dataGen.m_reported[ 4 ];
    }

    test = test / 100 / days;

    int stop = 0;


/*
    int roundsLeft = 99;
    while ( roundsLeft > 1 ) {
        int A;
        cin >> A;
        vector<int> advice( A );
        getVector( advice );
        int R;
        cin >> R;
        vector<int> recent( R );
        getVector( recent );
        int money;
        int timeLeft;
        cin >> money;
        cin >> timeLeft;
        cin >> roundsLeft;
        vector<int> ret = sol.getInvestments( advice, recent, money, timeLeft, roundsLeft );
        cout << ret.size() << endl;
        for ( int i = 0; i < (int)ret.size(); ++i )
            cout << ret[ i ] << endl;
        cout.flush();
    }
*/
}
