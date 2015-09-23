/** 
 BPlusTree.h 
 **/  
#pragma once
#include <stdio.h>

//======================================================================================  
class BPlusNode
{  
public:  
    BPlusNode* parent;  
    bool isLeafNode;  
  
//public:  
    BPlusNode( bool leaf=true ) {  
        parent = NULL;  
        isLeafNode = leaf;  
    }  
    ~BPlusNode() {}  
    bool& Leaf() {  
        return isLeafNode;  
    }  
    BPlusNode*& Parent() {  
        return this->parent;  
    }  
};  
  
//====================================================================================== 
template < typename KeyClass, int M >  
class BInternalNode: public BPlusNode  
{  
public:  
    static const int MaxKey = M-1;  
    static const int MinKey = (M+1)/2-1;  
  
//private:  
    KeyClass key[M+1];              //key count: = child count - 1;  
    BPlusNode* child[M+1];          //child count: = ceil[M/2] --> M;  
    int keyCnt;  
  
public:  
    BInternalNode(): BPlusNode(false) {  
        keyCnt = 0;  
    }  
    ~BInternalNode() {}  
    KeyClass& Key(int idx) { return key[idx]; }  
    BPlusNode*& Child(int idx) { return child[idx]; }  
    int& KeyCnt() { return keyCnt; }  
    bool IsOver() { return keyCnt > MaxKey; }  //key count: ceil(m/2)-1 --> M-1  
    bool IsTooLittle() { return keyCnt < MinKey; }  
  
    int Search( KeyClass const& key );  
    void Insert( KeyClass const& key, BPlusNode * pNode );  
    void Delete( int );  
};  
  
/** 
 *  find the child index where the key should insert  
 *  Maybe the name "SearchChild" is better! 
 */  
template < typename KeyClass, int M >  
int BInternalNode<KeyClass, M>::Search( KeyClass const& key ) {  
    int i, j, mid;  
    i = 0;   
    j = keyCnt-1;  
    while ( i < j ) {  
        mid = (i+j)>>1;  
        if ( key < this->key[mid] )  
            j = mid-1;  
        else if ( this->key[mid] < key )  
            i = mid+1;  
        else  
            break;  
    }  
    mid = i;  
    //judge:  
    if ( !( key < this->key[mid] ) ) {  
        return (mid+1);  
    }  
    return mid;  
}  
  
/* 
 just insert, no split 
 */  
template < typename KeyClass, int M >  
void BInternalNode<KeyClass, M>::Insert( KeyClass const& key, BPlusNode * pNode ) {  
    int i;  
    for ( i = keyCnt; i > 0; -- i ) {  
        if ( this->key[i-1] < key )  
            break;  
        this->key[i] = this->key[i-1];  
        this->child[i+1] = this->child[i];  
    }  
    this->key[i] = key;  
    this->child[i+1] = pNode;  
    ++ keyCnt;  
    //setting parent  
    pNode->Parent() = this;  
}  
  
/* 
Just delete key[index] and child[index+1] 
*/  
template < typename KeyClass, int M >  
void BInternalNode<KeyClass, M>::Delete( int idx )  
{  
    for ( int i = idx+1; i < keyCnt; ++ i ) {  
        key[i-1] = key[i];  
        child[i] = child[i+1];  
    }  
    -- keyCnt;  
}  
  
//======================================================================================  
template < typename KeyClass, typename ValueClass, int L >  
class BLeafNode: public BPlusNode  
{  
public:  
    static const int MaxKey = L;  
    static const int MinKey = (L+1)/2;  
  
//private:  
    KeyClass key[L+1];                  //key count == value count  
    ValueClass value[L+1];  
    BLeafNode* next;  
    int keyCnt;  
  
public:  
    BLeafNode(): BPlusNode(true) {  
        keyCnt = 0;  
        next = NULL;  
    }  
    ~BLeafNode() {}  
    KeyClass& Key(int idx) { return key[idx]; }  
    ValueClass& Value(int idx) { return value[idx]; }  
  
    BLeafNode*& Next() { return next; }  
    int& KeyCnt() { return keyCnt; }  
    bool IsOver() { return keyCnt > MaxKey; }  
    bool IsTooLittle() { return keyCnt < MinKey; }  
  
    int Search( KeyClass const& key );  
    void Insert( KeyClass const& key, ValueClass const& value );  
    void Delete( int );  
};  
  
/** 
 *  -1: not exist! 
 */  
template < typename KeyClass, typename ValueClass, int L >  
int BLeafNode<KeyClass,ValueClass,L>::Search( KeyClass const& key ) {  
    int i, j, mid;  
    i = 0;   
    j = keyCnt-1;  
    while ( i <= j ) {  
        mid = (i+j)>>1;  
        if ( key < this->key[mid] )  
            j = mid-1;  
        else if ( this->key[mid] < key )  
            i = mid+1;  
        else  
            return mid;  
    }  
    return -1;  
}  
  
/** 
 *  just insert key-value at specified position; 
 */  
template < typename KeyClass, typename ValueClass, int L >  
void BLeafNode<KeyClass,ValueClass,L>::Insert( KeyClass const& key, ValueClass const& value ) {  
    int i;  
    for ( i = keyCnt; i > 0; -- i ) {  
        if ( this->key[i-1] < key )  
            break;  
        this->key[i] = this->key[i-1];  
        this->value[i] = this->value[i-1];  
    }  
    this->key[i] = key;  
    this->value[i] = value;  
    ++ keyCnt;  
}  
  
/** 
 * 
 * 
 **/  
template < typename KeyClass, typename ValueClass, int L >  
void BLeafNode<KeyClass,ValueClass,L>::Delete( int idx )   
{  
    for ( int i = idx; i < keyCnt; ++ i )   
    {  
        key[i] = key[i+1];  
        value[i] = value[i+1];  
    }  
    -- keyCnt;  
}  
  
//======================================================================================  
template < typename KeyClass, typename ValueClass, int M, int L=M >  
class BPlusTree  
{  
public:  
    typedef BPlusNode           Node;  
    typedef BPlusNode*          PNode;  
    typedef BInternalNode<KeyClass,M>     InternalNode;  
    typedef BInternalNode<KeyClass,M>*        PInternalNode;  
    typedef BLeafNode<KeyClass, ValueClass, L>        LeafNode;  
    typedef BLeafNode<KeyClass, ValueClass, L>*       PLeafNode;  
  
//private:  
    PNode root;  
    PLeafNode head;  
  
public:  
    BPlusTree();  
    ~BPlusTree();  
  
    bool Search( KeyClass const& key, ValueClass *pValue );  
    bool Insert( KeyClass const& key, ValueClass const& value );  
    bool Delete( KeyClass const& key );  
  
    //just for test  
    void Print();  
    void PrintLevel();  
    void Print( PNode );  
private:  
    void Clear( PNode );  
    PLeafNode Search( KeyClass const& key, int& idx );  
    PInternalNode SearchInternal( KeyClass const& key, int& idx );  
    void SplitRootNode();  
    void SplitLeafNode( PLeafNode );  
    void SplitInternalNode( PInternalNode );  
  
    void Rebalance( PLeafNode );  
    void Rebalance( PInternalNode );  
    void MergeLeafNode( PLeafNode, int );  
    void MergeInternalNode( PInternalNode, int );  
};  
  
//=======================================================================================  
template < typename KeyClass, typename ValueClass, int M, int L >  
BPlusTree<KeyClass, ValueClass, M, L>::BPlusTree() {  
    root = NULL;  
}  
  
template < typename KeyClass, typename ValueClass, int M, int L >  
BPlusTree<KeyClass, ValueClass, M, L>::~BPlusTree() {  
    Clear( root );  
    root = head = NULL;  
}  
  
/** 
 *  Search whether it contains the specified key, if true, return the corresponding value. 
 *  otherwise, return false. 
 **/  
template < typename KeyClass, typename ValueClass, int M, int L >  
bool BPlusTree<KeyClass, ValueClass, M, L>::Search( KeyClass const& key, ValueClass *pValue ) {  
    int idx;  
    if ( NULL == root ) {  
        return false;             
    }  
    PNode p = (PNode)Search( key, idx );  
    if ( NULL == p || -1 == idx )  
        return false;  
    if ( NULL != pValue )  
        //*pValue = ((PLeafNode)p)->Value(idx);  
        *pValue = (static_cast<PLeafNode>(p))->Value(idx);  
    return true;  
}  
  
template < typename KeyClass, typename ValueClass, int M, int L >  
bool BPlusTree<KeyClass, ValueClass, M, L>::Insert( KeyClass const& key,  
        ValueClass const& value ) {  
  
    if ( NULL == root ) {  
        root = new LeafNode();  
        head = (PLeafNode)root;  
        ((PLeafNode)root)->Insert( key, value );  
    }  
    else {  
        int idx;  
        //idx: the position which key inserted in  
        PLeafNode p = Search( key, idx );  
  
        //the key exist!  
        if ( -1 != idx )   
            return false;  
        p->Insert( key, value );  
  
        if ( p->IsOver() )  
        {  
            if ( root == p )        //root must be a leafnode  
                SplitRootNode();  
            else  
                SplitLeafNode(p);  
        }  
    }  
    return true;  
}  
/**  
 *  1 Reach the leaf that contains the key.  
 *  2 Delete key from leaf:  
 *    remove the key from leaf node.  
 *    2.1 if internal node contains this key, replace it by the small key in the new leaf.  
 *    2.2 else   
 *        2.2.1 Lend a key from left sibling leaf.  
 *        2.2.2 Lend a key form right sibling leaf.  
 *        2.2.3 Merge leaf and its sibling.  
 *  for attention:  
 *      Keep the parent of leaf contains the smallest key in the leaf.  
 */  
template < typename KeyClass, typename ValueClass, int M, int L >  
bool BPlusTree<KeyClass, ValueClass, M, L>::Delete( KeyClass const& key ){  
    int idx;  
    PLeafNode p = Search( key, idx );  
    if ( NULL == p || idx < 0 )  
        return false;  
  
    p->Delete( idx );  
    if ( 0 == idx && root != p ) {  
        PInternalNode internal = SearchInternal( key, idx );  
        if ( NULL != internal && idx > -1 )  
            internal->Key(idx) = p->Key(0);  
    }  
  
    if ( p->IsTooLittle() && root != p )  
    {  
        Rebalance(p);  
    }  
    return true;  
}  
  
//just for test  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::Print() {  
    cout << "Print head list:" << endl;  
    PLeafNode p = head;  
    while ( NULL != p ) {  
        Print(p);  
        p = p->Next();  
    }  
    cout << endl;  
}  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::Print( PNode p ) {  
    cout << "(";  
    if ( true == p->Leaf() ) {  
        PLeafNode leaf = static_cast<PLeafNode>(p);  
        for ( int i = 0; i < leaf->KeyCnt(); ++ i )  
            cout << leaf->Key(i) << "  " << leaf->Value(i) << ", ";  
    }  
    else {  
        PInternalNode inter = static_cast<PInternalNode>(p);  
        for ( int i = 0; i < inter->KeyCnt(); ++ i )  
            cout << inter->Key(i) << " ";  
    }  
    cout << ")\n";  
}  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::PrintLevel() {  
    cout << "PrintLevel:" << endl;  
    if ( true == root->Leaf() ) {  
        cout << "[ ";  
        Print( root );  
        cout << " ]" << endl;  
        return;  
    }  
  
    int i, level;  
    queue<PNode> q;  
    PNode p;  
    q.push( root );  
    q.push( NULL );  
    level = 0;  
    cout << "Level :" << level << endl;  
    while ( !q.empty() ) {  
        p = q.front();  
        q.pop();  
        if ( NULL == p ) {  
            ++ level;  
            if ( !q.empty() )  
                q.push(NULL);  
            cout << endl << "Level :" << level << ends;  
            continue;  
        }  
        Print(p);  
        if ( true != p->Leaf() ) {  
            PInternalNode tmp = static_cast<PInternalNode>(p);  
            for ( i = 0; i <= tmp->KeyCnt(); ++ i )  
                q.push( tmp->Child(i) );  
        }  
    }  
    cout << endl;  
}  
  
//==============================================================================================================  
//private:  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::Clear( PNode p ) {  
    if ( NULL == p )  
        return;  
    if ( false == p->Leaf() ) {  
        PInternalNode inter = static_cast<PInternalNode>(p);  
        for ( int i = 0; i <= inter->KeyCnt(); ++ i ) {  
            Clear( inter->Child(i) );  
        }  
    }  
    delete p;  
}  
  
/** 
 *  If tree has the parameter key, the return the leafnode and key index; 
 *  otherwise, set idx = -1. 
 **/  
template < typename KeyClass, typename ValueClass, int M, int L >  
typename BPlusTree<KeyClass, ValueClass, M, L>::PLeafNode  
    BPlusTree<KeyClass, ValueClass, M, L>::Search( KeyClass const& key, int& result ){  
  
    if ( NULL == root )  
        return NULL;  
  
    PNode p = root;  
    if ( true == root->Leaf() )   
    {  
        result = ((PLeafNode)p)->Search(key);  
    }  
    else {  
        int idx;  
        PInternalNode internal;  
        //internal node  
        while ( NULL != p && p->Leaf() == false )   
        {  
            internal = static_cast<PInternalNode>(p);  
            idx = internal->Search(key);  
      
            p = internal->Child(idx);  
        }  
        //leaf node  
        //idx = ((PLeafNode)p)->Search(key);  
        result = static_cast<PLeafNode>(p)->Search(key);  
    }  
    return (PLeafNode)p;  
}  
  
/** 
 *  Search a internal node that contains the key! 
 * */  
template < typename KeyClass, typename ValueClass, int M, int L >  
typename BPlusTree<KeyClass, ValueClass, M, L>::PInternalNode  
    BPlusTree<KeyClass, ValueClass, M, L>::SearchInternal( KeyClass const& key, int& result ){  
      
    if ( NULL == root || true == root->Leaf() )  
        return NULL;  
    PNode p = root;  
    int idx;  
    PInternalNode internal;  
  
    while ( NULL != p && p->Leaf() == false )   
    {  
        internal = static_cast<PInternalNode>(p);  
        idx = internal->Search(key);  
        if ( idx > 0 && internal->Key(idx-1) == key ) {  
            result = idx-1;  
            return internal;  
        }  
        p = internal->Child(idx);  
    }  
    result = -1;  
    return NULL;  
}  
  
//root must be a leafnode  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::SplitRootNode()   
{  
    PLeafNode pRNode;  
    PLeafNode p = (PLeafNode)root;  
    int i, left, right;  
      
    pRNode = new LeafNode();  
    left = (L+1)>>1;  
    right = p->KeyCnt() - left;  
    for ( i = 0; i < right; ++ i ) {  
        pRNode->Key(i) = p->Key(i+left);  
        pRNode->Value(i) = p->Value(i+left);  
    }  
    pRNode->Next() = NULL;  
    p->Next() = pRNode;  
  
    pRNode->KeyCnt() = right;  
    p->KeyCnt() = left;  
  
    PInternalNode newRoot = new InternalNode();  
    p->Parent() = newRoot;  
    pRNode->Parent() = newRoot;  
  
    newRoot->Key(0) = pRNode->Key(0);  
    newRoot->Child(0) = root;  
    newRoot->Child(1) = pRNode;  
    newRoot->KeyCnt() = 1;  
  
    //head = (BPlusNode)root;  
    root = newRoot;  
}  
  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::SplitLeafNode( PLeafNode p )   
{  
    PLeafNode pRNode = new LeafNode();  
    int i, left, right;  
  
    left = (L+1)>>1;  
    right = p->KeyCnt() - left;  
    for ( i = 0; i < right; ++ i ) {  
        pRNode->Key(i) = p->Key(i+left);  
        pRNode->Value(i) = p->Value(i+left);  
    }  
    pRNode->Next() = p->Next();  
    p->Next() = pRNode;  
  
    pRNode->KeyCnt() = right;  
    p->KeyCnt() = left;  
  
    if ( NULL !=p->Parent() ) {  
        PInternalNode tmp = static_cast<PInternalNode>( p->Parent() );  
        tmp->Insert( pRNode->Key(0), pRNode );  
        if ( tmp->IsOver() ) {  
            SplitInternalNode( tmp );  
        }  
    }  
    else {  
        PInternalNode newRoot = new InternalNode();  
        newRoot->KeyCnt() = 1;  
        newRoot->Child(0) = p;  
        newRoot->Child(1) = pRNode;  
        newRoot->Key(0) = pRNode->Key(0);  
        p->Parent() = newRoot;  
        pRNode->Parent() = newRoot;  
  
        root = newRoot;  
    }  
}  
  
/* 
SplitInternalNode() 
    1 Node ==> Left( ceil(M/2)-1 ) + Key(ÖÐ¼ä) + Right( M/2 £© 
    2 InsertInternalNode( Key, Right ) 
*/  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::SplitInternalNode( PInternalNode p )   
{  
    int leftK, rightK, i;  
    KeyClass midkey;  
  
    leftK = (M+1)/2-1;  
    rightK = M/2;  
    midkey = p->Key(leftK);  
  
    PInternalNode pRNode = new InternalNode();  
    for ( i = 0; i < rightK; ++ i ) {  
        pRNode->Key(i) = p->Key( i+leftK+1 );  
        pRNode->Child(i) = p->Child( i+leftK+1 );  
    }  
    pRNode->Child(rightK) = p->Child( p->KeyCnt() );  
    pRNode->KeyCnt() = rightK;  
    p->KeyCnt() = leftK;  
  
    for ( i = 0; i < pRNode->KeyCnt()+1; ++ i ) {  
        pRNode->Child(i)->Parent() = pRNode;  
    }  
  
    if ( NULL != p->Parent() ) {  
        PInternalNode tmp = static_cast<PInternalNode>( p->Parent() );  
        tmp->Insert( midkey, pRNode );  
        if ( tmp->IsOver() ) {  
            SplitInternalNode( tmp );  
        }  
    }  
    else {  
        PInternalNode newRoot = new InternalNode();  
        newRoot->KeyCnt() = 1;  
        newRoot->Child(0) = p;  
        newRoot->Child(1) = pRNode;  
        newRoot->Key(0) = midkey;  
        p->Parent() = newRoot;  
        pRNode->Parent() = newRoot;  
  
        root = newRoot;  
    }  
}  
  
/* 
*/  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::Rebalance( PLeafNode p )  
{  
    int index, i;  
    PInternalNode pa;  
    PLeafNode prev, next;  
  
    pa = static_cast<PInternalNode>( p->Parent() );  
    index = pa->Search( p->Key(0) );      //index: child pointer index!  
  
    //attention:   
    // prev and next node must have the same parent node!  
    prev = next = NULL;  
    if ( index != 0 )  
        prev = static_cast<PLeafNode>( pa->Child(index-1) );  
    if ( index < pa->KeyCnt() )  
        next = static_cast<PLeafNode>( pa->Child(index+1) );  
  
    if ( NULL != next && next->KeyCnt() > LeafNode::MinKey ) {  
  
        p->Key( p->KeyCnt() ) = next->Key(0);      // pa->Key(index) == next->Key(0)  
        p->Value( p->KeyCnt() ) = next->Value(0);  
        ++ p->KeyCnt();  
  
        for ( i = 1; i < next->KeyCnt(); ++ i ) {  
            next->Key(i-1) = next->Key(i);  
            next->Value(i-1) = next->Value(i);  
        }  
        -- next->KeyCnt();  
  
        pa->Key( index ) = next->Key(0);  
    }  
    else if ( NULL != prev && prev->KeyCnt() > LeafNode::MinKey ) {  
          
        for ( i = p->KeyCnt()-1; i >= 0; -- i ) {  
            p->Key(i+1) = p->Key(i);  
            p->Value(i+1) = p->Value(i);  
        }  
  
        p->Key(0) = prev->Key( prev->KeyCnt()-1 );       
        p->Value(0) = prev->Value( prev->KeyCnt()-1 );  
        ++ p->KeyCnt();  
        -- prev->KeyCnt();  
  
        pa->Key(index-1) = p->Key(0);  
    }  
    else {  
        MergeLeafNode( p, index );  
    }  
}  
  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::Rebalance( PInternalNode p )  
{  
    int index, i;  
    PInternalNode pa, prev, next;  
  
    pa = static_cast<PInternalNode>( p->Parent() );  
    index = pa->Search( p->Key(0) );      //index: child pointer index!  
    prev = next = NULL;  
  
    if ( index < pa->KeyCnt() )  
        next = static_cast<PInternalNode>( pa->Child(index+1) );  
    if ( index > 0 )  
        prev = static_cast<PInternalNode>( pa->Child(index-1) );  
  
    if ( NULL != next && next->KeyCnt() > InternalNode::MinKey ) {  
  
        p->Key( p->KeyCnt() ) = pa->Key( index );        
  
        pa->Key( index ) = next->Key(0);  
  
        p->Child( p->KeyCnt()+1 ) = next->Child(0);  
        p->Child( p->KeyCnt()+1 )->Parent() = p;  
        ++ p->KeyCnt();  
  
        for ( i = 1; i < next->KeyCnt(); ++ i ) {  
            next->Key(i-1) = next->Key(i);  
            next->Child(i-1) = next->Child(i);  
        }  
        next->Child( next->KeyCnt()-1 ) = next->Child( next->KeyCnt() );  
        -- next->KeyCnt();  
    }  
    else if ( NULL != prev && prev->KeyCnt() > InternalNode::MinKey ) {  
          
        for ( i = p->KeyCnt()-1; i >= 0; -- i ) {  
            p->Key(i+1) = p->Key(i);  
            p->Child(i+2) = p->Child(i+1);  
        }  
        p->Child(1) = p->Child(0);  
  
        p->Key(0) = pa->Key( index-1 );  
        pa->Key( index-1 ) = prev->Key( prev->KeyCnt()-1 );  
  
        p->Child(0) = prev->Child( prev->KeyCnt() );  
        p->Child(0)->Parent() = p;  
  
        ++ p->KeyCnt();  
        -- prev->KeyCnt();  
    }  
    else {  
        MergeInternalNode( p, index );  
    }  
}  
  
/////////////////////////////////////////////////////////////////  
/* 
 p->Parent()->Child(idx) == p  
 */  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::MergeLeafNode( PLeafNode p, int idx )   
{  
    PLeafNode next, prev, first, second;  
    PInternalNode pa;  
    int i, pos;  
  
    pos = -1;  
    pa = static_cast<PInternalNode>( p->Parent() );  
    prev = next = NULL;  
    //attention: prev and next must have the same parent node.  
    if ( idx != 0 )  
        prev = static_cast<PLeafNode>( pa->Child(idx-1) );  
    if ( idx < pa->KeyCnt() )  
        next = static_cast<PLeafNode>( pa->Child(idx+1) );  
  
    if ( NULL != next ) {  
        pos = idx;  
        first = p;  
        second = next;  
    }  
    else if ( idx > 0 ) {          
        pos = idx-1;  
        first = prev;  
        second = p;  
    }  
  
    for ( i = 0; i < second->KeyCnt(); ++ i ) {  
        first->Key( first->KeyCnt()+i ) = second->Key(i);  
        first->Value( first->KeyCnt()+i ) = second->Value(i);  
    }  
    first->KeyCnt() += second->KeyCnt();  
    first->Next() = second->Next();  
  
    pa->Delete( pos );           //remove key[pos] and child[pos+1]  
    delete second;  
  
    if ( root == pa && 0 == pa->KeyCnt() ) {  
        root = first;  
        delete pa;  
    }  
    else if ( pa->IsTooLittle() && root != pa ) {  
        Rebalance( pa );  
    }  
}  
  
/* 
 p->Parent()->Child(idx) == p  
 */  
template < typename KeyClass, typename ValueClass, int M, int L >  
void BPlusTree<KeyClass, ValueClass, M, L>::MergeInternalNode( PInternalNode p, int idx )   
{  
    PInternalNode next, prev, first, second, pa;  
    int i, pos;  
  
    pa = static_cast<PInternalNode>( p->Parent() );  
    prev = next = NULL;  
    pos = -1;  
    if ( idx < pa->KeyCnt() )  
        next = static_cast<PInternalNode>( pa->Child(idx+1) );  
    if ( idx > 0 )  
        prev = static_cast<PInternalNode>( pa->Child(idx-1) );  
  
    if ( NULL != next ) {  
        pos = idx;  
        first = p;  
        second = next;  
    }  
    else {  
        pos = idx-1;  
        first = prev;  
        second = p;  
    }  
      
  
    first->Key( first->KeyCnt() ) = pa->Key( pos );  
    for ( i = 0; i < second->KeyCnt(); ++ i ) {  
        first->Key( first->KeyCnt()+1+i ) = second->Key(i);  
        first->Child( first->KeyCnt()+1+i ) = second->Child(i);  
        second->Child(i)->Parent() = first;  
    }  
    first->KeyCnt() += second->KeyCnt() + 1;  
    first->Child( first->KeyCnt() ) = second->Child( second->KeyCnt() );  
    second->Child( second->KeyCnt() )->Parent() = first;  
  
    pa->Delete( pos );           //remove key[pos] and child[pos+1]  
    delete second;  
      
    if ( root == pa && 0 == pa->KeyCnt() ) {  
        root = first;  
        delete pa;  
    }  
    else if ( pa->IsTooLittle() && root != pa ) {  
        Rebalance( pa );  
    }  
}  
  