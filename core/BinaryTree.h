#pragma once

#include "core.h"
#include "Lockable.h"

#include <stdlib.h>

template <typename T> class BinaryTree:public Lockable
{
public:
    BinaryTree():Lockable()
    {
        _init();
    }
    BinaryTree(const BinaryTree& that):Lockable()
    {
        _init();
        copy(that);
    }
    virtual ~BinaryTree()
    {
        if(_root)delete _root;
    }

    BinaryTree& operator=(const BinaryTree& that)
    {
        return copy(that);
    }

    BinaryTree& operator+=(const BinaryTree& that)
    {
        return add(that);
    }

    BinaryTree& operator-=(const BinaryTree& that)
    {
        return sub(that);
    }

    bool operator==(const BinaryTree& that)const
    {
        if(count()!=that.count())return false;
        return _containsAll(_root,that);
    }

    bool operator!=(const BinaryTree& that)const
    {
        return !(*this==that);
    }

    BinaryTree& copy(const BinaryTree& that)
    {
        that.copyTo(this);
        return *this;
    }

    void copyTo(BinaryTree* that)const
    {
        that->clear();
        if(_root)that->_root=new _BinaryTreeNode(_root,that);
    }

    BinaryTree& add(const BinaryTree& that,bool replace=false)
    {
        that.addTo(this,replace);
        return *this;
    }

    void addTo(BinaryTree* that,bool replace=false)const
    {
        if(that->isEmpty())copyTo(that);
        else _addTo(that,_root,replace);
    }

    BinaryTree& sub(const BinaryTree& that)
    {
        that.subTo(this);
        return *this;
    }

    void subTo(BinaryTree* that)const
    {
        if((*this)==(*that))that->clear();
        else if(!that->isEmpty())
            _subTo(that,_root);
    }

    virtual int compareItems(const T* t1,const T* t2)const
    {
        return t1-t2;
    }

    bool autoDelete()const
    {
        return _autoDelete;
    }
    void setAutoDelete(bool autoDelete)
    {
        _autoDelete=autoDelete;
    }

    inline int count()const
    {
        return _count;
    }
    inline bool isEmpty()const
    {
        return _count==0;
    }

    T* find(T* t)
    {
        _currentNode=_find(t);
        return current();
    }

    T* insert(T* t,bool replace=false)
    {
        return _BinaryTreeNode::insert(t,this,&_root,replace);
    }

    T* take(T* t=NULL)
    {
        if(t==NULL)t=current();
        if(t==NULL)return NULL;
        _BinaryTreeNode* node=_BinaryTreeNode::remove(t,&_root);
        if(node)
        {
            T* data=node->data();
            node->setData(NULL);
            delete node;
            return data;
        }
        return NULL;
    }

    bool remove(T* t=NULL)
    {
        if(t==NULL)t=current();
        if(t==NULL)return false;
        _BinaryTreeNode* node=_BinaryTreeNode::remove(t,&_root);
        if(node)
        {
            delete node;
            return true;
        }
        return false;
    }

    void walk(void (*fct)(const T* t))const
    {
        _walk(_root,fct);
    }

    void walk(void (T::*fct)())const
    {
        _walk(_root,fct);
    }

    void walk(void (T::*fct)()const)const
    {
        _walk(_root,fct);
    }

    T* min()const
    {
        _BinaryTreeNode* crtNode=_BinaryTreeNode::firstNode(_root);
        return crtNode?crtNode->data():NULL;
    }

    T* max()const
    {
        _BinaryTreeNode* crtNode=_BinaryTreeNode::lastNode(_root);
        return crtNode?crtNode->data():NULL;
    }

    void clear()
    {
        if(_root)delete _root;
        _root=NULL;
        _currentNode=NULL;
    }

    inline T* first()
    {
        _currentNode=_BinaryTreeNode::firstNode(_root);
        return current();
    }

    inline T* last()
    {
        _currentNode=_BinaryTreeNode::lastNode(_root);
        return current();
    }

    inline T* current()const
    {
        return _currentNode?_currentNode->data():NULL;
    }

    inline T* next()
    {
        if(_currentNode==NULL)return NULL;
        if(_currentNode->right())
        {
            _currentNode=_currentNode->right();
            while(_currentNode->left())
                _currentNode=_currentNode->left();
        }
        else
        {
            _BinaryTreeNode* lastNode=NULL;
            while(_currentNode&&_currentNode->right()==lastNode)
            {
                lastNode=_currentNode;
                _currentNode=_currentNode->up();
            }
            if(_currentNode&&_currentNode->left()!=lastNode)
                while(_currentNode->left())
                    _currentNode=_currentNode->left();
        }
        return current();
    }

    inline T* previous()
    {
        if(_currentNode==NULL)return NULL;
        if(_currentNode->left())
        {
            _currentNode=_currentNode->left();
            while(_currentNode->right())
                _currentNode=_currentNode->right();
        }
        else
        {
            _BinaryTreeNode* lastNode=NULL;
            while(_currentNode&&_currentNode->left()==lastNode)
            {
                lastNode=_currentNode;
                _currentNode=_currentNode->up();
            }
            if(_currentNode&&_currentNode->right()!=lastNode)
                while(_currentNode->right())
                    _currentNode=_currentNode->right();
        }
        return current();
    }

protected:
    class _BinaryTreeNode
    {
        friend class BinaryTree;
    public:
        _BinaryTreeNode(T* data,BinaryTree* tree)
        {
            _init(data,tree);
        }
        _BinaryTreeNode(const _BinaryTreeNode* that,BinaryTree* tree)
        {
            _init(that->data(),tree,that->balance());
            if(that->left())setLeft(new _BinaryTreeNode(that->left(),tree));
            if(that->right())setRight(new _BinaryTreeNode(that->right(),tree));
        }
        virtual ~_BinaryTreeNode()
        {
            if(_left)delete _left;
            if(_right)delete _right;
            if(_tree->autoDelete()&&_data)delete _data;
            _tree->_decCount();
        }

        inline BinaryTree* tree()const
        {
            return _tree;
        }
        inline T* data()const
        {
            return _data;
        }
        inline void setData(T* data)
        {
            _data=data;
        }

        inline int balance()const
        {
            return _balance;
        }
        inline void setBalance(int balance)
        {
            _balance=balance;
        }

        inline _BinaryTreeNode* left()const
        {
            return _left;
        }
        inline void setLeft(_BinaryTreeNode* left)
        {
            _left=left;
            if(_left)_left->setUp(this);
        }
        inline _BinaryTreeNode* right()const
        {
            return _right;
        }
        inline void setRight(_BinaryTreeNode* right)
        {
            _right=right;
            if(_right)_right->setUp(this);
        }
        inline _BinaryTreeNode* up()const
        {
            return _up;
        }
        inline void setUp(_BinaryTreeNode* up)
        {
            _up=up;
        }

    protected:
        inline void _isolate()
        {
            setLeft(NULL);
            setRight(NULL);
            setUp(NULL);
            setBalance(0);
            if(this==tree()->_currentNode)
                tree()->_currentNode=NULL;
        }

    public:
        static T* insert(T* data,BinaryTree* tree,_BinaryTreeNode** node,bool replaceData=false,int* balance=NULL)
        {
            if((*node)==NULL)
            {
                (*node)=new _BinaryTreeNode(data,tree);
                tree->_currentNode=(*node);
                if(balance)(*balance)=1;
                return data;
            }
            int cmp=tree->compareItems((*node)->data(),data);
            _BinaryTreeNode* replacementNode=NULL;
            int balanceDelta=0;
            if(cmp<0)
            {
                replacementNode=(*node)->right();
                data=insert(data,tree,&replacementNode,replaceData,&balanceDelta);
                (*node)->setRight(replacementNode);
            }
            else if(cmp>0)
            {
                replacementNode=(*node)->left();
                data=insert(data,tree,&replacementNode,replaceData,&balanceDelta);
                (*node)->setLeft(replacementNode);
                balanceDelta=-balanceDelta;
            }
            else
            {
                if(replaceData)
                {
                    if(tree->autoDelete())
                        delete (*node)->data();
                    (*node)->setData(data);
                }
                if(balance)(*balance)=0;
                return (*node)->data();
            }
            if(balanceDelta==0)
            {
                if(balance)(*balance)=0;
            }
            else
            {
                (*node)->setBalance((*node)->balance()+balanceDelta);
                (*node)=_balanceNode(*node);
                if(balance)(*balance)=((*node)->balance()==0?0:1);
            }
            return data;
        }

        static _BinaryTreeNode* remove(T* data,_BinaryTreeNode** node,int* balance=NULL)
        {
            if((*node)==NULL)
            {
                if(balance)(*balance)=0;
                return NULL;
            }
            int cmp=(*node)->tree()->compareItems((*node)->data(),data);
            _BinaryTreeNode* replacementNode=NULL;
            _BinaryTreeNode* removedNode=NULL;
            int balanceDelta=0;
            if(cmp<0)
            {
                replacementNode=(*node)->right();
                removedNode=remove(data,&replacementNode,&balanceDelta);
                (*node)->setRight(replacementNode);
            }
            else if(cmp>0)
            {
                replacementNode=(*node)->left();
                removedNode=remove(data,&replacementNode,&balanceDelta);
                (*node)->setLeft(replacementNode);
                balanceDelta=-balanceDelta;
            }
            else if((*node)->left()==NULL)
            {
                replacementNode=(*node)->right();
                if(replacementNode)replacementNode->setUp(NULL);
                removedNode=(*node);
                removedNode->_isolate();
                (*node)=replacementNode;
                if(balance)(*balance)=-1;
                return removedNode;
            }
            else if((*node)->right()==NULL)
            {
                replacementNode=(*node)->left();
                if(replacementNode)replacementNode->setUp(NULL);
                removedNode=(*node);
                removedNode->_isolate();
                (*node)=replacementNode;
                if(balance)(*balance)=-1;
                return removedNode;
            }
            else
            {
                _BinaryTreeNode* firstNode=_BinaryTreeNode::firstNode((*node)->right());
                data=(*node)->data();
                (*node)->setData(firstNode->data());
                firstNode->setData(data);
                replacementNode=(*node)->right();
                removedNode=_removeLastNode(&replacementNode,&balanceDelta);
                (*node)->setRight(replacementNode);
            }
            if(balanceDelta==0)
            {
                if(balance)(*balance)=0;
            }
            else
            {
                (*node)->setBalance((*node)->balance()+balanceDelta);
                (*node)=_balanceNode(*node);
                if(balance)(*balance)=((*node)->balance()==0?-1:0);
            }
            return removedNode;
        }

    protected:
        static inline _BinaryTreeNode* _removeLastNode(_BinaryTreeNode** node,int* balance=NULL)
        {
            _BinaryTreeNode* removedNode=NULL;
            if((*node)->left()==NULL)
            {
                removedNode=(*node);
                (*node)=(*node)->right();
                if(*node)(*node)->setUp(NULL);
                removedNode->_isolate();
                if(balance)(*balance)=-1;
                return removedNode;
            }
            _BinaryTreeNode* replacementNode=(*node)->left();
            int balanceDelta=0;
            removedNode=_removeLastNode(&replacementNode,&balanceDelta);
            (*node)->setLeft(replacementNode);
            if(balanceDelta==0)
            {
                if(balance)(*balance)=0;
            }
            else
            {
                balanceDelta=-balanceDelta;
                (*node)->setBalance((*node)->balance()+balanceDelta);
                (*node)=_balanceNode(*node);
                if(balance)(*balance)=((*node)->balance()==0?-1:0);
            }
            return removedNode;
        }

        static _BinaryTreeNode* _balanceNode(_BinaryTreeNode* node)
        {
            if(node->balance()>1)
            {
                if(node->right()->balance()>=0)
                {
                    return _rotateLeft(node);
                }
                else
                {
                    node->setRight(_rotateRight(node->right()));
                    return _rotateLeft(node);
                }
            }
            else if(node->balance()<-1)
            {
                if(node->left()->balance()<=0)
                {
                    return _rotateRight(node);
                }
                else
                {
                    node->setLeft(_rotateLeft(node->left()));
                    return _rotateRight(node);
                }
            }
            return node;
        }

        static _BinaryTreeNode* _rotateLeft(_BinaryTreeNode* node)
        {
            int balance=node->balance();
            _BinaryTreeNode* right=node->right();
            int rightBalance=right->balance();
            node->setRight(right->left());
            right->setLeft(node);
            node->setBalance(balance-MAX(rightBalance,0)-1);
            right->setBalance(MIN(MIN(balance-2,balance+rightBalance-2),rightBalance-1));
            right->setUp(NULL);
            return right;
        }

        static _BinaryTreeNode* _rotateRight(_BinaryTreeNode* node)
        {
            int balance=node->balance();
            _BinaryTreeNode* left=node->left();
            int leftBalance=left->balance();
            node->setLeft(left->right());
            left->setRight(node);
            node->setBalance(balance-MIN(leftBalance,0)+1);
            left->setBalance(MAX(MAX(balance+2,balance+leftBalance+2),leftBalance+1));
            left->setUp(NULL);
            return left;
        }

    public:
        static inline _BinaryTreeNode* firstNode(_BinaryTreeNode* node)
        {
            if(node==NULL)return NULL;
            while(node->left())
                node=node->left();
            return node;
        }

        static inline _BinaryTreeNode* lastNode(_BinaryTreeNode* node)
        {
            if(node==NULL)return NULL;
            while(node->right())
                node=node->right();
            return node;
        }

    protected:
        virtual void _init(T* data,BinaryTree* tree,int balance=0)
        {
            _tree=tree;
            _left=_right=_up=NULL;
            _balance=balance;
            setData(data);
            _tree->_incCount();
        }

        _BinaryTreeNode *_left,*_right,*_up;
        int _balance;
        BinaryTree* _tree;
        T* _data;
    };

protected:
    void _addTo(BinaryTree* that,const _BinaryTreeNode* crtNode,bool replace=false)const
    {
        if(crtNode==NULL)return;
        that->insert(crtNode->data(),replace);
        _addTo(that,crtNode->left(),replace);
        _addTo(that,crtNode->right(),replace);
    }

    void _subTo(BinaryTree* that,const _BinaryTreeNode* crtNode)const
    {
        if(crtNode==NULL)return;
        _subTo(that,crtNode->left());
        _subTo(that,crtNode->right());
        that->remove(crtNode->data());
    }

    bool _containsAll(const _BinaryTreeNode* crtNode,const BinaryTree& that)const
    {
        if(crtNode==NULL)return true;
        if(that._find(crtNode->data())==NULL)return false;
        return _containsAll(crtNode->left(),that)&&_containsAll(crtNode->right(),that);
    }

    _BinaryTreeNode* _find(const T* t)const
    {
        _BinaryTreeNode* crtNode=_root;
        while(crtNode)
        {
            int cmp=compareItems(t,crtNode->data());
            if(cmp==0)return crtNode;
            crtNode=(cmp<0?crtNode->left():crtNode->right());
        }
        return NULL;
    }

    void _walk(_BinaryTreeNode* crtNode,void (*fct)(const T* t))const
    {
        if(crtNode==NULL)return;
        _walk(crtNode->left(),fct);
        fct(crtNode->data());
        _walk(crtNode->right(),fct);
    }

    void _walk(_BinaryTreeNode* crtNode,void (T::*fct)())const
    {
        if(crtNode==NULL)return;
        _walk(crtNode->left(),fct);
        (crtNode->data()->*fct)();
        _walk(crtNode->right(),fct);
    }

    void _walk(_BinaryTreeNode* crtNode,void (T::*fct)()const)const
    {
        if(crtNode==NULL)return;
        _walk(crtNode->left(),fct);
        (crtNode->data()->*fct)();
        _walk(crtNode->right(),fct);
    }

protected:
    int _incCount()
    {
        return ++_count;
    }
    int _decCount()
    {
        return --_count;
    }

    void _init()
    {
        _root=NULL;
        _currentNode=NULL;
        _autoDelete=false;
        _count=0;
    }

protected:
    _BinaryTreeNode* _root;
    _BinaryTreeNode* _currentNode;
    bool _autoDelete;
    int _count;

    friend class _BinaryTreeNode;
};
