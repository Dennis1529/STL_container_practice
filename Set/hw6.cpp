#ifndef SET_H
#define SET_H

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <iomanip>
using std::setw;

#include <set>

enum Color{ Red, Black };

// TreeNode class template definition
template< typename Kty >
class TreeNode
{
   template< typename K > friend class set;
   template< typename K > friend class SetIterator;
private:
   Kty myval;
   TreeNode *left;   // left subtree, or smallest element if head
   TreeNode *parent; // parent, or root of tree if head
   TreeNode *right;  // right subtree, or largest element if head
   Color color;      // red or black, black if head
   bool isNil;       // true only if head (also nil) node
}; // end class template TreeNode

// SetIterator class template definition
template< typename Kty >
class SetIterator
{
   template< typename K > friend class set;
public:
   // default constructor
   SetIterator( TreeNode< Kty > *p = NULL )
      : ptr( p )
   {
   }

   // destructor
   ~SetIterator()
   {
   }

   // overloaded dereference operator
   Kty& operator*() const
   {
      return ptr->myval;
   }

   // overloaded class member access operator
   Kty* operator->() const
   {
      return &( ptr->myval );
   }

   // move to next node in inorder traversal
   SetIterator& operator++()
   {
      TreeNode< Kty > *node;
      if( ( ptr->right )->isNil )
      {
         node = ptr->parent;
         while( !node->isNil && node->myval < ptr->myval )
            node = node->parent;
      }
      else
      {
         node = ptr->right;
         while( !( node->left )->isNil )
            node = node->left;
      }

      ptr = node;
      return *this;
   }

   // overloaded equality operator
   bool operator==( const SetIterator &right ) const
   {
      return ptr == right.ptr;
   }

   // overloaded inequality operator
   bool operator!=( const SetIterator &right ) const
   {
      return ptr != right.ptr;
   }

private:
   TreeNode< Kty > *ptr; // keep a pointer to set
}; // end class template SetIterator


// set class template definition
template< typename Kty >
class set
{
public:
   typedef SetIterator< Kty > iterator;

   // Constructs an empty set object, with head node.
   set()
      : myHead( new TreeNode< Kty > ),
        mySize( 0 )
   {
      myHead->left = myHead;
      myHead->parent = myHead;
      myHead->right = myHead;
      myHead->color = Black;
      myHead->isNil = true;
   }

   // Destroys the set object.
   ~set()
   {
      clear( myHead->parent );
      delete myHead;
   }

   // Returns an iterator referring to the node with smallest key
   const iterator begin() const
   {
      return SetIterator< Kty >( myHead->left );
   }

   // Returns an iterator referring to the past-the-end element in the set container.
   const iterator end() const
   {
      return SetIterator< Kty >( myHead );
   }

   // Extends the container by inserting new elements,
   // effectively increasing the container size by the number of elements inserted.
   void insert( const Kty& val )
   {
	   TreeNode< Kty > *s= new TreeNode<Kty>;
	   s->isNil = false;
	   s->right = s->left = myHead;
	   s->myval = val;
	   if(mySize == 0)
	   {	   		   
		   s->color = Black;
		   s->parent = myHead;
		   myHead->parent= myHead->left = myHead->right = s;
	   }
	   else
	   {		   
		   TreeNode<Kty> *tmp = myHead->parent, *leftlast = myHead->parent, *rightlast = myHead->parent;
		   s->color = Red;
		   while (true)
		   {
			   if (val > tmp->myval)
			   {
				   if (tmp->right == myHead)
				   {
					   tmp->right = s;
					   s->parent = tmp;
					   tmp = s;
					   break;
				   }
				   else
					   tmp=tmp->right;
			   }
			   else
			   {
				   if (tmp->left == myHead)
				   {
					   tmp->left = s;
					   s->parent = tmp;
					   tmp = s;
					   break;
				   }
				   else
					   tmp = tmp->left;
			   }
		   }
		   while (leftlast->left != myHead) //跑到tree 最左下
		   {
			   leftlast = leftlast->left;
		   }
		   myHead->left = leftlast;
		   while (rightlast->right != myHead) // 跑到tree 最右下
		   {
			   rightlast = rightlast->right;
		   }
		   myHead->right = rightlast;
		   reBalance(tmp);
	   }   
	   mySize++;
   }

   bool isRBTree()
   {
      TreeNode< Kty > *root = myHead->parent;

      // computes the number of black nodes in the path from the root to the node with smallest key
      int numBlackNodes1 = 0;
      for( TreeNode< Kty > *node = root; node != myHead; node = node->left )
         if( node->color == Black )
            numBlackNodes1++;

      int numBlackNodes2 = 0;
      if( root->color == Black )
         return isRBSubtree( root, numBlackNodes1, numBlackNodes2 );
      else
         return false;
   }

   void twoTraversals()
   {
      cout << "Preorder sequence:\n";
      preorder( myHead->parent );
      cout << endl;

      cout << "Inorder sequence:\n";
      for( iterator it = begin(); it != end(); ++it )
         cout << *it << endl;
      cout << endl;
   }

   bool operator==( std::set< Kty > &data )
   {
      unsigned int size = *( reinterpret_cast< unsigned int * >( &data ) + 2 );

      if( size != mySize )
         return false;

      unsigned int *head = *( reinterpret_cast< unsigned int ** >( &data ) + 1 );
      unsigned int color = static_cast< unsigned int >( *( head + 3 ) ) % 256;
      if( color != myHead->color )
         return false;

      bool isNil = static_cast< unsigned int >( *( head + 3 ) ) / 256 % 256;
      if( isNil != myHead->isNil )
         return false;

      if( size == 0 )
         return true;

      unsigned int *root = *( reinterpret_cast< unsigned int ** >( head + 1 ) );
      return equalTree( root, head, myHead->parent );
   }

private:
   TreeNode< Kty > *myHead; // pointer to head node
   unsigned int mySize;     // number of elements ( not include head node )

   // Removes all elements from the set object (which are destroyed)
   void clear( TreeNode< Kty > *node )
   {
      if( !node->isNil ) // node is not an external node
      {
         clear( node->left );
         clear( node->right );
         delete node;
      }
   }

   void reBalance( TreeNode< Kty > *node )
   {
	   if (node->parent == myHead)
		   node->color = Black;
	   else if (node->parent->parent == myHead)
		   return;
	   else
	   {
		   TreeNode<Kty> *u = node;
		   TreeNode<Kty> *pu = node->parent;
		   TreeNode<Kty> *gu = node->parent->parent;
		   if (pu->color == Red && u->color == Red)
		   {
			   if (gu->left == pu)
			   {
				   if (gu->right->color == Red)//LLr LRr
				   {
					   pu->color = Black;
					   if(gu!=myHead->parent)
							gu->color = Red;					   
					   gu->right->color = Black;
					   reBalance(gu);
				   }
				   else
				   {
					   if (pu->left == u)					   
						   LLbRotation(u);					   
					   else					   
						   LRbRotation(u);					   
				   }
			   }
			   else
			   {
				   if (gu->left->color == Red)//RLr RRr
				   {
					   pu->color = Black;
					   if (gu != myHead->parent)
							gu->color = Red;					   
					   gu->left->color = Black;
					   reBalance(gu);
				   }
				   else
				   {
					   if (pu->left == u)					   
						   RLbRotation(u);					   
					   else
						   RRbRotation(u);					   
				   }
			   }
		   }
		   else
			   return;
	   }
   }

   void LLbRotation( TreeNode< Kty > *node )
   {
	   TreeNode<Kty> *u = node;
	   TreeNode<Kty> *pu = node->parent;
	   TreeNode<Kty> *gu = node->parent->parent;
	   pu->color = Black;
	   gu->color = Red;
	   pu->parent = gu->parent;
	   if (gu->parent == myHead)
		   myHead->parent = pu;
	   else //gu->parent 非myHead 即為node 要指right left
		   (gu->parent->left == gu) ? gu->parent->left = pu : gu->parent->right = pu;
	   gu->parent = pu;
	   if (pu->right != myHead) pu->right->parent = gu;//pu->right若不為myHead 代表有Node 要將此node的parent 指向gu
	   gu->left = pu->right;
	   pu->right = gu;
	   //pu->left = u;
	   reBalance(pu);
   }

   void LRbRotation( TreeNode< Kty > *node )
   {
	   TreeNode<Kty> *u = node;
	   TreeNode<Kty> *pu = node->parent;
	   TreeNode<Kty> *gu = node->parent->parent;
	   u->color = Black;
	   gu->color = Red;
	   u->parent = gu->parent;
	   if (gu->parent == myHead)
		   myHead->parent = u;
	   else//gu->parent 非myHead 即為node 要重指gu->parent的 right left
	       (gu->parent->left == gu) ? gu->parent->left = u : gu->parent->right = u;
	   gu->parent = pu->parent = u;
	   pu->right = u->left;
	   if (u->left != myHead)//u->left若不為myHead 代表有Node 要將此node的parent 指向pu
		   u->left->parent = pu;
	   gu->left = u->right;
	   if (u->right != myHead)//u->right若不為myHead 代表有Node 要將此node的parent 指向gu
		   u->right->parent = gu;
	   u->right = gu;
	   u->left = pu;
	   reBalance(u);
   }

   void RLbRotation( TreeNode< Kty > *node )
   {
	   TreeNode<Kty> *u = node;
	   TreeNode<Kty> *pu = node->parent;
	   TreeNode<Kty> *gu = node->parent->parent;
	   u->color = Black;
	   gu->color = Red;
	   u->parent = gu->parent;
	   if (gu->parent == myHead)
		   myHead->parent = u;
	   else//gu->parent 非myHead 即為node 要重指gu->parent的 right left
	       (gu->parent->left == gu) ? gu->parent->left = u : gu->parent->right = u;
	   gu->parent = pu->parent = u;
	   pu->left = u->right;
	   if (u->right != myHead)//u->right若不為myHead 代表有Node 要將此node的parent 指向pu
		   u->right->parent = pu;
	   gu->right = u->left;
	   if (u->left != myHead)//u->left若不為myHead 代表有Node 要將此node的parent 指向gu
		   u->left->parent = gu;
	   u->left = gu;
	   u->right = pu;
	   reBalance(u);
   }

   void RRbRotation( TreeNode< Kty > *node )
   {
	   TreeNode<Kty> *u = node;
	   TreeNode<Kty> *pu = node->parent;
	   TreeNode<Kty> *gu = node->parent->parent;
	   pu->color = Black;
	   gu->color = Red;
	   if (gu->parent == myHead)
		   myHead->parent = pu;
	   else//gu->parent 非myHead 即為node 要重指gu->parent的 right left
		   (gu->parent->left == gu) ? gu->parent->left = pu : gu->parent->right = pu;
	   pu->parent = gu->parent;	   
	   gu->parent = pu;
	   gu->right = pu->left;
	   if (pu->left != myHead) pu->left->parent = gu;//pu->left若不為myHead 代表有Node 要將此node的parent 指向gu
	   pu->left = gu;
	   //pu->right = u;
	   reBalance(pu);
   }

   bool isRBSubtree( TreeNode< Kty > *node, int numBlackNodes1, int numBlackNodes2 )
   {
      if( node == myHead ) // node is an external node
         return ( numBlackNodes1 == numBlackNodes2 ); // property RB3
      else
      {  // node violate the property for binary search trees
         if( ( node->left != myHead && ( node->left )->myval > node->myval ) ||
            ( node->right != myHead && node->myval > ( node->right )->myval ) )
            return false;
         else
            if( node->color == Black )
               return isRBSubtree( node->left, numBlackNodes1, numBlackNodes2 + 1 ) &&
                      isRBSubtree( node->right, numBlackNodes1, numBlackNodes2 + 1 );
            else // node->color == Red
            {
               if( ( node->left )->color == Red || ( node->right )->color == Red )
                  return false; // violate property RB2

               return isRBSubtree( node->left, numBlackNodes1, numBlackNodes2 ) &&
                      isRBSubtree( node->right, numBlackNodes1, numBlackNodes2 );
            }
      }
   }

   void preorder( TreeNode< Kty > *node )
   {
      if( node != myHead )
      {
         cout << setw( 3 ) << node->myval << "  ";
         if( node->color == Red )
            cout << "Red\n";
         else
            cout << "Black\n";

         preorder( node->left );
         preorder( node->right );
      }
   }

   bool equalTree( unsigned int *node, unsigned int *head, TreeNode< Kty > *myNode )
   {
      if( node == head && myNode == myHead )
         return true;

      if( node == head && myNode != myHead )
         return false;

      if( node != head && myNode == myHead )
         return false;

      unsigned int key = *( node + 4 );
      if( key != myNode->myval )
         return false;

      unsigned int color = static_cast< unsigned int >( *( node + 3 ) ) % 256;
      if( color != myNode->color )
         return false;

      bool isNil = static_cast< unsigned int >( *( node + 3 ) ) / 256 % 256;
      if( isNil != myNode->isNil )
         return false;

      unsigned int *leftChild = *( reinterpret_cast< unsigned int ** >( node ) );
      unsigned int *rightChild = *( reinterpret_cast< unsigned int ** >( node + 2 ) );

      return equalTree( leftChild, head, myNode->left ) &&
             equalTree( rightChild, head, myNode->right );
   }
}; // end class template set

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
         keys[ i ] = 1 + rand() % ( 5 * numKeys );
      while( duplicate( keys, i ) );

   set< unsigned int > data1;
   std::set< unsigned int > data2;
   for( unsigned int i = 0; i < numKeys; ++i )
   {
      data1.insert( keys[ i ] );
      data2.insert( keys[ i ] );
   }

   if( data1.isRBTree() )
      cout << "data1 contains a correct Red-Black tree\n\n";
   else
      cout << "data1 does not contain a correct Red-Black tree\n\n";

   if( data1 == data2 )
      cout << "data1 == data2\n\n";
   else
      cout << "data1 != data2\n\n";

   //data1.twoTraversals();

   system( "pause" );
}