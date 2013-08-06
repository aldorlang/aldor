#include <stdio.h>
#include "db_cc.h"

template<class T>
class List {
    class Cell {
	friend class List<T>;
	T *item;
	Cell *next;
    public:
	Cell(T *i): item(i), next(0) {}
	Cell(T *i, Cell *ptr): item(i), next(ptr) {}
	~Cell() { 
	    if (item) delete item;
	    if (next) delete next;
	}
    };
    Cell *fst;
public:
    List(): fst(0) {}

    ~List() { 
	if (fst) delete fst;
	fst = 0;
    }

    void insert(T *obj) {
	if (!fst)
	    fst = new Cell(obj);
	else
	    fst = new Cell(obj, fst);
    }

    T *first() { return fst->item; }

    void gotoNext() { if (fst) fst = fst->next; }

    int Empty() { return (fst == 0); }
};

class DBPrettyComplex {
    List<ItemPrettyComplex> *l;
public:
    DBPrettyComplex() { l = new List<ItemPrettyComplex>(); }

    ~DBPrettyComplex() { if (l) delete(l); }

    void insert(ItemPrettyComplex *ipc) {
	l->insert(ipc);
    }

    void display() {
	while (!(l->Empty())) {
	    l->first()->display();
	    l->gotoNext();
	}
    }
};


int main() {
    DBComplex *db;
    DBPrettyComplex *dbp;

    printf("C++ side calling Aldor function f\n");
    f();
    printf("End of Aldor function f\n\n");

    printf("Now let's use the classes ...\n");
    db = DBComplex::bracket();
    db->insert(ItemComplex::bracket(25,50));
    db->insert(ItemComplex::bracket(30,60));
    db->insert(ItemComplex::bracket(40,80));
    db->insert(ItemComplex::bracket(12,24));
    db->insert(ItemComplex::bracket(8,16));
    db->display();
    printf("End of use of classes\n\n");

    printf("DB pretty complex\n");
    dbp = new DBPrettyComplex();
    dbp->insert(ItemPrettyComplex::bracket(1,2,"A"));
    dbp->insert(ItemPrettyComplex::bracket(6,3,"B"));
    dbp->insert(ItemPrettyComplex::bracket(12,8,"C"));
    dbp->insert(ItemPrettyComplex::bracket(51,62,"D"));
    dbp->insert(ItemPrettyComplex::bracket(34,87,"E"));
    dbp->insert(ItemPrettyComplex::bracket(231,322,"F"));
    dbp->display();
    printf("End DB pretty complex\n");
}
