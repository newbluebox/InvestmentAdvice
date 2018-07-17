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
#include <limits>

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


const double PI = 3.14159265359;

double pnormal( double x, double mu, double sigma )
{
    double z = ( x - mu ) / sigma;
    return std::exp( -z * z / 2.0 ) / std::sqrt( 2 * PI * sigma * sigma );
}


class InvestmentAdvice
{

public:
    InvestmentAdvice() :
        m_round( 0 ), m_expl( 100 )
    {
    }
public:

    double expectmax( const vector< double > & b, const vector< double > & a, double & w, double & var, int n )
    {
        //int n = m_round;

        //double w = m_p[ expert ];
        //double var = m_var[ expert ];

        //vector< double > b = m_b[ expert ];
        //vector< double > a = m_a[ expert ];

        double oldL = 0;
        double newL = -std::numeric_limits<float>::infinity();

        double delta = 0.0001;

        vector< double > g1( n, 0 );
        vector< double > g2( n, 0 );
        vector< double > g1a( n, 0 );

        for ( int i = 0; i < n; ++i )
        {
            g1a[ i ] = pnormal( a[ i ], 0, 0.1 );
            g2[ i ] = pnormal( b[ i ], 0, 0.1 ) * g1a[ i ];
        }

        vector< double > gamma( n, 0 );
        do
        {
            oldL = newL;
            newL = 0;
            double gamma_sum = 0;
            for ( int i = 0; i < n; ++i )
            {
                g1[ i ] = pnormal( b[ i ], a[ i ], std::sqrt( var ) ) * g1a[ i ];
                double den = w * g1[ i ] + ( 1 - w ) * g2[ i ];
                gamma[ i ] = w * g1[ i ] / den;
                gamma_sum += gamma[ i ];
                newL += std::log( den );
            }

            newL = newL / n;
            w = gamma_sum / n;

            var = 0;

            for ( int i = 0; i < n; ++i )
                var += gamma[ i ] * ( b[ i ] - a[ i ] ) * ( b[ i ] - a[ i ] );

            var = var / gamma_sum;

        } while ( std::abs( oldL - newL ) > delta );

        //m_p[ expert ] = w;
        //m_var[ expert ] = var;

        return newL;
    }

    void multistart_expectmax( int expert )
    {
        int n = m_round;

        double w = m_p[ expert ];
        double var = m_var[ expert ];

        vector< double > b = m_b[ expert ];
        vector< double > a = m_a[ expert ];

        double ll = expectmax( b, a, w, var, n );
        std::uniform_real_distribution<>dis_1( 0, 1 );

        for ( int i = 0; i < 5; ++i )
        {
            double w_ = dis_1( gen );
            double var_ = dis_1( gen ) * 0.2;
            var_ *= var_;

            double newll = expectmax( b, a, w_, var_, n );

            if ( newll > ll )
            {
                ll = newll;
                w = w_;
                var = var_;
            }
        }

        m_p[ expert ] = w;
        m_var[ expert ] = var;
    }

    void estimate_a_prob( int expert, double bi )
    {
        double w = m_p[ expert ];
        double var = m_var[ expert ];
    }

    vector<int> getInvestments( vector<int> advice, vector<int> recent, int money, int timeLeft, int roundsLeft )
    {
        if ( m_round == 0 )
        {
            m_numExperts = (int)advice.size();
            m_totalRounds = roundsLeft + 1;
            init();
        }

        for ( int i = 0; i < m_numExperts; ++i )
            m_b[ i ][ m_round ] = advice[ i ] / 100.0;

        if ( m_round > 0 )
        {
            for ( int i = 0; i < m_numExperts; ++i )
                m_a[ i ][ m_round - 1 ] = ((double)recent[ i ]) / m_expl;
        }

        vector<int> ret( advice.size() );
        for ( int i = 0; i < advice.size(); i++ )
        {
                ret[ i ] = m_expl;
        }

        if ( m_round > 5 )
        {
            for ( int i = 0; i < m_numExperts; ++i )
                multistart_expectmax( i );
        }

        m_round++;
        return ret;
    }

private:
    void init()
    {
        for ( int i = 0; i < m_numExperts; ++i )
        {
            m_b.push_back( vector< double >( m_totalRounds, 0 ) );
            m_a.push_back( vector< double >( m_totalRounds, 0 ) );

            m_p.push_back( 0.5 );
            m_var.push_back( 0.01 );
        }
    }

private:
    int m_round;
    int m_numExperts;
    int m_totalRounds;
    int m_expl;
    vector< vector< double > > m_b;
    vector< vector< double > > m_a;

    vector< double > m_p;
    vector< double > m_var;
};

// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector( vector<T>& v ) {
    for ( int i = 0; i < v.size(); ++i )
        cin >> v[ i ];
}

int main() {
    InvestmentAdvice sol;
    DataGenerator dataGen( 33 );

    int days = 100;

    for ( int i = 0; i < days; ++i )
    {
        dataGen.generateNextDay();
        vector< int > invest = sol.getInvestments( dataGen.m_reported, dataGen.m_result, dataGen.m_money, 0, days - i - 1 );
        dataGen.applyResults( invest );
    }


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
