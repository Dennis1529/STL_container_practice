#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <iomanip>
using std::setw;

#include <vector>
using std::vector;

#include <list>
using std::list;

#include <unordered_set>


#ifndef UNORDERED_SET
#define UNORDERED_SET

// unordered_set class template definition
template< typename T >
class unordered_set
{
public:
   unordered_set();
   ~unordered_set();

   // Returns the number of elements in the unordered_set container.
   unsigned int size() const;

   // Searches the container for an element with k as value and
   // returns an iterator to it if found,
   // otherwise it returns an iterator to unordered_set::end
   // (the element past the end of the container).
   typename list< T >::iterator find( const T &k );

   // Inserts a new element in the unordered_set.
   // The element is inserted only if it is not equivalent to any other element
   // already in the container ( elements in an unordered_set have unique values ).
   // This effectively increases the container size by one.
   void insert( const T &val );

   // Returns the number of buckets in the unordered_set container.
   unsigned int bucket_count() const;

   // Returns the number of elements in bucket n.
   unsigned int bucket_size( unsigned int n ) const;

   // Returns the bucket number where the element with value k is located.
   unsigned int bucket( const T &k ) const;

   // Checks if the current object and data is the same
   void verify( std::unordered_set< T > &data );

private:
   list< T > myList;	   // list of elements, must initialize before myVec

   // vector of list iterators, begin() then end() - 1
   vector< typename list< T >::iterator > myVec;

   unsigned int maxidx = 8; // current maximum key value

   // put a new element in the unordered_set when myVec is large enough
   void putIn( const T& val );

   // Computes hash value which will be used to compute bucket number
   unsigned int hashSeq( const unsigned int &key, unsigned int count ) const;
}; // end class template unordered_set


template< typename T >
unordered_set< T >::unordered_set()
{
   myVec.reserve( 16 );
   myVec.assign( 16, myList.end() );
}

template< typename T >
unordered_set< T >::~unordered_set()
{
   myList.clear();
   myVec.clear();
}

template< typename T >
unsigned int unordered_set< T >::size() const
{
   return myList.size();
}

template< typename T >
typename list< T >::iterator unordered_set< T >::find( const T &k )
{
	list< T >::iterator it;
	for (it = myList.begin(); it != myList.end(); ++it)
	{
		if (*it == k)
			return it;
	}
	return it;
}

template< typename T >
void unordered_set< T >::insert( const T& val )
{	
	if (find(val) == myList.end())
	{
		putIn(val);
		if (size() > bucket_count())
		{
			if (maxidx < 512)
			{
				maxidx = maxidx * 8;
			}
			else
			{
				maxidx = maxidx * 2;
			}
			myVec.reserve(2 * maxidx);
			myVec.assign(2 * maxidx, myList.end());
			list<T> myCopyList = myList;
			list<T>::iterator it1 = myCopyList.begin();
			list<T>::iterator it2 = myList.begin();
			for (; it1 != myCopyList.end(); ++it1)
			{
				unsigned int Bucket = 2 * bucket(*it1);
				if (myVec[Bucket] == myList.end())
				{
					myVec[Bucket] = myVec[Bucket + 1] = it2;
					++it2;
				}
				else
				{
					list<T>::iterator _list_it_temp = it2;
					++it2;
					myList.erase(_list_it_temp);
					putIn(*it1);
				}
			}
		}
	}
}

template< typename T >
unsigned int unordered_set< T >::bucket_count() const
{
   return maxidx;
}

template< typename T >
unsigned int unordered_set< T >::bucket_size( unsigned int n ) const
{	// return size of bucket n
	unsigned int numbers = 0;
	if (n < maxidx)
		if (myVec[2 * n] != myList.end())
		{
			numbers = 1;
			for (list<T>::iterator it = myVec[2 * n]; it != myVec[2 * n + 1]; ++it)
				numbers++;
		}
	return numbers;
	/*if (n < maxidx)
	{
		if (n == 0)
			return 1;
		else
		{
			if (myVec[n * 2 - 2] == myList.end())
				return 0;
			else
			{
				list<T>::iterator it = myVec[n * 2 - 2];
				int i;
				for (i = 0; it != myVec[n * 2 - 1]; ++it)
				{
					i++;
				}
				return i + 1;
			}
		}
	}*/
}

template< typename T >
unsigned int unordered_set< T >::bucket( const T &k ) const
{
   unsigned int hashValue = hashSeq( k, sizeof( T ) );
   return hashValue % maxidx; // bucket number
}

template< typename T >
void unordered_set< T >::putIn( const T& val )
{
	if (myVec[bucket(val) * 2] == myList.end())
	{
		myVec[bucket(val) * 2] = myVec[bucket(val) * 2 + 1] = myList.insert(myList.end(), val);
	}
	else
	{
		list<T>::iterator it = myVec[bucket(val) * 2];
		myVec[bucket(val) * 2]=myList.insert(it, val);
	}
}

template< typename T >
unsigned int unordered_set< T >::hashSeq( const unsigned int &key, unsigned int count ) const
{
   const unsigned int _FNV_offset_basis = 2166136261U; // 10000001 00011100 10011101 11000101
   const unsigned int _FNV_prime = 16777619U;          // 00000001 00000000 00000001 10010011
   const unsigned char *first = reinterpret_cast< const unsigned char * >( &key );
   unsigned int val = _FNV_offset_basis;
   for( unsigned int next = 0; next < count; ++next )
   {	// fold in another byte
      val ^= static_cast< unsigned int >( first[ next ] );
      val *= _FNV_prime;
   }

   return val;
}

template< typename T >
void unordered_set< T >::verify( std::unordered_set< T > &data )
{
   unsigned int numErrors = 0;
   unsigned int *firstVec = *( reinterpret_cast< unsigned int ** >( &data ) + 5 );
   vector< list< T >::iterator >::iterator it = myVec.begin();
   for( unsigned int bucketNo = 0; it != myVec.end(); ++it, bucketNo++ )
   {
      unsigned int *stlBucketFirst = *( reinterpret_cast< unsigned int ** >( &firstVec[ 2 * bucketNo ] ) );
      unsigned int *stlBucketLast = *( reinterpret_cast< unsigned int ** >( &firstVec[ 2 * bucketNo + 1 ] ) );

      list< T >::iterator myBucketFirst = *it;
      ++it;
      list< T >::iterator myBucketLast = *it;
      if( myBucketFirst != myList.end() )
      {
         cout << "bucketNo:" << setw( 5 ) << bucketNo << "  ";
         if( myBucketFirst == myBucketLast )
         {
            cout << "   myKey:" << setw( 5 ) << *myBucketFirst
                 << "   stlKey:" << setw( 5 ) << *( stlBucketFirst + 2 );
            if( *myBucketFirst != *( stlBucketFirst + 2 ) )
            {
               cout << "   error";
               numErrors++;
            }
         }
         else
         {
            unsigned int *stlPtr = stlBucketFirst;
            list< T >::iterator myIt = myBucketFirst;
            while( myIt != myBucketLast )
            {
               cout << "   myKey:" << setw( 5 ) << *myIt
                    << "   stlKey:" << setw( 5 ) << *( stlPtr + 2 );
               if( *myIt != *( stlPtr + 2 ) )
               {
                  cout << "   error";
                  numErrors++;
               }
               cout << ";  ";
               stlPtr = *( reinterpret_cast< unsigned int ** >( stlPtr ) );
               ++myIt;
            }
            cout << "   myKey:" << setw( 5 ) << *myBucketLast
                 << "   stlKey:" << setw( 5 ) << *( stlBucketLast + 2 );
            if( *myBucketLast != *( stlBucketLast + 2 ) )
            {
               cout << "   error";
               numErrors++;
            }
         }
         cout << endl;
      }
   }

   cout << "\nThere are totally " << numErrors << " errors.\n\n";
}

#endif

bool duplicate( unsigned int *keys, unsigned int i )
{
   for( unsigned int j = 0; j < i; j++ )
      if( keys[ j ] == keys[ i ] )
         return true;
   return false;
}

int main()
{
   unsigned int seed;
   cout << "Enter the seed for rand: ";
   cin >> seed;
   srand( seed );

   unsigned int numKeys;
   cout << "Enter the number of keys: ";
   cin >> numKeys;
   cout << endl;

   unsigned int *keys = new unsigned int[ numKeys ];
   for( unsigned int i = 0; i < numKeys; i++ )
      do
         keys[ i ] = rand() % ( 5 * numKeys );
      while( duplicate( keys, i ) );

   for( unsigned int i = 0; i < numKeys; i++ )
      cout << "keys:" << setw( 6 ) << keys[ i ] << endl;

   unordered_set< unsigned int > data1;
   std::unordered_set< unsigned int > data2;

   for( unsigned int i = 0; i < numKeys; i++ )
   {
      data1.insert( keys[ i ] );
      data2.insert( keys[ i ] );
   }

/*
   for( unsigned int key = 0; key < numKeys; key++ )
   {
      data1.insert( key );
      data2.insert( key );
   }
*/
   for( unsigned int key = 0; key < data1.size(); key++ )
      if( data1.bucket( key ) != data2.bucket( key ) )
         cout << setw( 6 ) << key << setw( 6 ) << data2.bucket( key )
                                  << setw( 6 ) << data1.bucket( key ) << endl;

   for( unsigned int bucketNo = 0; bucketNo < data1.bucket_count(); bucketNo++ )
      if( data1.bucket_size( bucketNo ) != data2.bucket_size( bucketNo ) )
         cout << setw( 6 ) << bucketNo << setw( 6 ) << data2.bucket_size( bucketNo )
              << setw( 6 ) << data1.bucket_size( bucketNo ) << endl;

   data1.verify( data2 );

   system( "pause" );
}
