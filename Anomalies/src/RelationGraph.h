#ifndef RELATION_GRAPH_H
#define RELATION_GRAPH_H
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//General includes
#include <vector>
#include <memory>
#include <list>
#include <ostream>

//General definitions

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class t>
struct RelationNodeObject {
  t  data_;

  RelationNodeObject(t lbl):data_(lbl){}
};

////////////////////////////////////////////////////////////////////////////////

template <class tSubData, class tObjData, class tEdgeData>
struct RelationNodeSubject {
  //definitions................................................................
  typedef RelationNodeObject <tObjData>                       objectType;

  typedef RelationNodeSubject <tSubData, tObjData, tEdgeData> subjecType;

  typedef std::pair<objectType, tEdgeData>                    objectPair;

  //main data..................................................................
  tSubData                data_;

  std::vector<objectPair> objectList_;

  //constructors destructors....................................................

  RelationNodeSubject(tSubData data) :data_(data) {}
  
  //main functions..............................................................
  void addObjectRelation(tObjData obj, tEdgeData edgeData){
    tObjData newObject(obj);
    objectList_.push_back(objectPair(newObject, edgeData));
  }
};

//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class ts, class to, class te>
struct RelationGraph {
  //definitions
  typedef RelationNodeSubject<ts, to, te> node;
  
  typedef std::list<node>                 lstNodeType;
  
  //variables
  lstNodeType listNodes_;

  //constructors destructors....................................................

  RelationGraph() { }
  ~RelationGraph() { }

  //main functions..............................................................
  void addSubjectNode(ts data) {
    listNodes_.push_back(node(data));
  }

  void addObjectRelation(to objData, te edgeData) {
    assert(!listNodes_.empty());
    auto last = listNodes_.rbegin();
    last->addObjectRelation(objData, edgeData);
  }
  void saving2os(std::ostream & os) {
    for (auto & no : listNodes_) {
      os << "N " << no.data_ << std::endl;
      for (auto & link : no.objectList_) {
        os << "E " << link.second << std::endl;
        os << "O " << link.first.data_ << std::endl;
      }
    }
  }
};

#endif //RELATION_GRAPH_H
