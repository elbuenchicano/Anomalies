#include "Support2.h"
#include <iomanip>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void anomaly1_vectorBuild(Observed &graph, set<AnomalyGt> &ans);
bool testLevel3(  string & set_file, short frm_step,
                  map<string, int>  *objs,
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs,
                  set<int>          &obs,
                  int               anomalytype) {
  
  FileStorage fs(set_file, FileStorage::READ);
//..............................................................................
  string  directory,
          token,
          gt_dir,
          out_matching,
          out_val_file;
  
  double limit,
         prec,
         rec;
    
  fs["load_graph_token"]       >> token;
  fs["load_graph_directory"]   >> directory;
  fs["testing3_out_file"]      >> out_matching;
  fs["testing3_out_val_file"]  >> out_val_file;
  fs["testing3_gt_file"]       >> gt_dir;
  fs["testing3_treshold"]      >> limit;
  fs["testing3_prec"]          >> prec;
  fs["testing3_recall"]        >> rec;

  cutil_file_cont graph_files;
  list<Observed>  graphs_training;

  cout << "Loading training graphs...\n";
  list_files(graph_files, directory.c_str(), token.c_str());
  for (auto & file : graph_files) {
    cout << file << endl;
    loadDescribedGraphs(file, graphs_training, &obs);
  }

  set<AnomalyGt> Anomalies_obs,
                 Anomalies_gt;

  fstream        outFile, out_val;
  outFile.open(out_matching, ios::out);
  out_val.open(out_val_file, ios::out | ios::app);

  list<Observed> graphs_test_Af;
//..............................................................................
  for (auto graph : graphs) {
    cout << "Processing first test graph!" << endl;
    matchNodes(graph, graphs_training, anomalytype, limit);
    AnomaliesSet(graph, Anomalies_obs);
    graphs_test_Af.push_back(graph);
    for (auto iter = Anomalies_obs.begin(); iter != Anomalies_obs.end(); iter++) {
      outFile << "Anomalie Frame  " << iter->ini_ << " to Frame " << iter->fin_ << endl;
    }
  }
  
  validateGraphList(graphs_test_Af, gt_dir, out_val_file,
    pair<double, double>(prec, rec), anomaly1_vectorBuild);
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
double matchNodes(Observed &graph_test, list<Observed> &graphs_training, int anomalytype, double limit) {
  
  for (auto it = graph_test.graph_.listNodes_.begin(); it != graph_test.graph_.listNodes_.end(); it++) {
    it->data_.end_ = it->data_.id_;
  }
  
  list<graphType> listGraphs_test, listGraphs_train;
  if (graph_test.graph_.listNodes_.size() > 2)
    splitGraph(graph_test, listGraphs_test, 3);
  else
    listGraphs_test.push_back(graph_test.graph_);

  double match_cost = 0.0, matchs = 0.0, distance = 0.0;
  int adj = 0, i = 0, Anom = 0, aux = 0, use = 0, backto = 0;
  
  //-----------------------------------------------------------------------------
  for (auto graph_test_splited : listGraphs_test) {
    vector<double> dist_values;
    int Anom = 1;
    for (auto graph_train : graphs_training) {
      for (auto it = graph_train.graph_.listNodes_.begin(); it != graph_train.graph_.listNodes_.end(); it++) {
        it->data_.end_ = it->data_.id_;
      }
      listGraphs_train.clear();
      if (graph_train.graph_.listNodes_.size() > 2)
        splitGraph(graph_train, listGraphs_train, 3);
      else
        listGraphs_train.push_back(graph_train.graph_);
//----------------------------------------------------------------------------- 
      for (auto graph_train_splited : listGraphs_train) {
        auto ittr = graph_train_splited.listNodes_.begin();
        int linha = 0;

        Matrix<double> costMatrix(graph_train_splited.listNodes_.size(),
          graph_test_splited.listNodes_.size());

        Munkres<double> Hung;

        for (auto node_train : graph_train_splited.listNodes_) {
          int coluna = 0;
          auto itte = graph_test_splited.listNodes_.begin();
          for (auto node_test : graph_test_splited.listNodes_) {
            match_cost = 0;
            if (itte == graph_test_splited.listNodes_.begin()) {

            }
            //-----------------------------Degree Comp-----------------------------------------------
            match_cost += degreeComparison(node_train.objectList_.size(), node_test.objectList_.size());
            //---------------------Adjacencies and Edge Comp-----------------------------------------------
            for (auto obj_train : node_train.objectList_) {
              for (auto obj_test : node_test.objectList_) {
                match_cost += adjacenciesComparison(obj_train, obj_test, adj);
              }
              if (adj == 0)
                match_cost += 1;
              adj = 0;
            }
            for (auto obj_test : node_test.objectList_) {
              for (auto obj_train : node_train.objectList_) {
                if (obj_train.first.data_.id_ == obj_test.first.data_.id_)
                  adj = 1;
              }
              if (adj == 0)
                match_cost += 1;
              adj = 0;
            }
        /*    //----------------------------------Neighbor Comp------------------        
            auto lim = 99999;
            if (lim < 0) {
            Neighbor:
              if (ittr->objectList_.size() != itte->objectList_.size())
                match_cost += 1;//abs(ittr->objectList_.size() - itte->objectList_.size());
              //-----------------------------------------------------------------------------
              for (auto obj_train : ittr->objectList_) {
                for (auto obj_test : itte->objectList_) {
                  if (obj_train.first.data_.id_ == obj_test.first.data_.id_) {
                    adj = 1;
                    /*if (obj_train.second > obj_test.second)
                      if (obj_train.second == 0)
                        match_cost += abs(obj_train.second - obj_test.second) / 1;
                      else
                        match_cost += abs(obj_train.second - obj_test.second) / obj_train.second;
                    else
                      if (obj_test.second == 0)
                        match_cost += abs(obj_test.second - obj_train.second) / 1;
                      else
                        match_cost += abs(obj_test.second - obj_train.second) / obj_test.second;
                  /*}
                }
                if (adj == 0)
                  match_cost += 1;
                adj = 0;
              }
              for (auto obj_test : itte->objectList_) {
                for (auto obj_train : ittr->objectList_) {
                  if (obj_train.first.data_.id_ == obj_test.first.data_.id_)
                    adj = 1;
                }
                if (adj == 0)
                  match_cost += 1;
                adj = 0;
              }
              if (backto == 1)
                goto back1;
              else if (backto == 2)
                goto back2;
              else if (backto == 3)
                goto back3;
              else if (backto == 4)
                goto back4;
              else if (backto == 5)
                goto back5;
              else if (backto == 6)
                goto back6;
              else if (backto == 7)
                goto back7;
            }
            if (itte == graph_test_splited.listNodes_.begin()) {
              if (ittr == graph_train_splited.listNodes_.begin()) {
                if (graph_test_splited.listNodes_.size() == 1 && graph_train_splited.listNodes_.size() > 1) {
                  ittr++;
                  match_cost += ittr->objectList_.size() + 1;
                  ittr--;
                }
                else if (graph_train_splited.listNodes_.size() == 1 && graph_test_splited.listNodes_.size() > 1) {
                  itte++;
                  match_cost += itte->objectList_.size() + 1;
                  itte--;
                }
              }
              else if (++ittr == graph_train_splited.listNodes_.end()) {
                ittr--;
                if (graph_train_splited.listNodes_.size() > 1) {
                  ittr--;
                  match_cost += ittr->objectList_.size() + 1;
                  ittr++;
                }
              }
              else {
                ittr--;
                if (graph_test_splited.listNodes_.size() == 1) {
                  ittr--;
                  match_cost += ittr->objectList_.size() + 1;
                  ittr++; ittr++;
                  match_cost += ittr->objectList_.size() + 1;
                  ittr--;
                }
                else {
                  ittr--;
                  match_cost += ittr->objectList_.size() + 1;
                  ittr++; ittr++; itte++;
                  backto = 1;
                  goto Neighbor;
                back1:
                  ittr--; itte--;
                }
              }
            }
            else if (++itte == graph_test_splited.listNodes_.end()) {
              itte--;
              if (ittr == graph_train_splited.listNodes_.begin()) {
                if (graph_train_splited.listNodes_.size() > 1) {
                  ittr++;
                  match_cost += ittr->objectList_.size() + 1;
                  ittr--;
                }
                itte--;
                match_cost += itte->objectList_.size() + 1;
                itte++;
              }
              else if (++ittr == graph_train_splited.listNodes_.end()) {
                ittr--;
                if (graph_train_splited.listNodes_.size() == 1) {
                  itte--;
                  match_cost += itte->objectList_.size() + 1;
                  itte++;
                }
                else {
                  itte--; ittr--;
                  backto = 2;
                  goto Neighbor;
                back2:
                  itte++; ittr++;
                }
              }
              else {
                // Nao precisa porque passou pelo else if e ja incrementou i ittr ---> ittr++;
                match_cost += ittr->objectList_.size() + 1;
                ittr--; ittr--; itte--;
                backto = 3;
                goto Neighbor;
              back3:
                ittr++; itte++;
              }
            }
            else {
              itte--;
              if (ittr == graph_train_splited.listNodes_.begin()) {
                if (graph_train_splited.listNodes_.size() == 1) {
                  itte--;
                  match_cost += itte->objectList_.size() + 1;
                  itte++; itte++;
                  match_cost += itte->objectList_.size() + 1;
                  itte--;
                }
                else {
                  itte--;
                  match_cost += itte->objectList_.size() + 1;
                  itte++; itte++; ittr++;
                  backto = 4;
                  goto Neighbor;
                back4:
                  itte--; ittr--;
                }
              }
              else if (++ittr == graph_train_splited.listNodes_.end()) {
                ittr--;
                itte++;
                match_cost += itte->objectList_.size() + 1;
                itte--; itte--; ittr--;
                backto = 5;
                goto Neighbor;
              back5:
                itte++; ittr++;
              }
              else {
                ittr--;
                ittr++; itte++;
                backto = 6;
                goto Neighbor;
              back6:
                ittr--; ittr--; itte--; itte--;
                backto = 7;
                goto Neighbor;
              back7:
                ittr++; itte++;

              }
            }*/
            neighborComparison(graph_train_splited, graph_test_splited, ittr, itte);
            //---------------------------------End Neighbor Comp----------------
            costMatrix(linha, coluna) = match_cost;
            coluna++;
            itte++;
          }
          linha++;
          ittr++;
        }
        i++;
        distance = getDistance(costMatrix, use, graph_train_splited.listNodes_.size(),
                               graph_test_splited.listNodes_.size(), matchs);
        dist_values.push_back(distance);
      }
    }
    for (int j = 0; j < dist_values.size(); j++) {
      if (dist_values[j] < limit) {
        Anom = 0;
        break;
      }
    }
    if (Anom == 1) {
      auto itsplit = graph_test_splited.listNodes_.begin();
      if (graph_test_splited.listNodes_.size() > 1) {
        itsplit++;
        for (auto itaux = graph_test.graph_.listNodes_.begin(); itaux != graph_test.graph_.listNodes_.end(); itaux++) {
          if (itaux->data_.id_ == itsplit->data_.id_)
            itaux->data_.anomaly_ = true;
          graph_test.levels_[anomalytype] = true;
        }
      }
    }
    Anom = 1;
  }
  return 1.0;
}


double degreeComparison(int train_node, int test_node) {
  if (train_node != test_node)
    return abs(train_node - test_node);
  return 0;
}

double adjacenciesComparison(pair<RelationNodeObject<ActorLabel>, double> & obj_train, pair<RelationNodeObject<ActorLabel>, double> &obj_test, int& adj) {
  double cost = 0;
  if (obj_train.first.data_.id_ == obj_test.first.data_.id_) {
    adj = 1;
    /*if (obj_train.second > obj_test.second) {
      if (obj_train.second == 0)
        cost = abs(obj_train.second - obj_test.second) / 1;
      else
        cost = abs(obj_train.second - obj_test.second) / obj_train.second;
    }
    else {
      if (obj_test.second == 0)
        cost = abs(obj_test.second - obj_train.second) / 1;
      else
        cost = abs(obj_test.second - obj_train.second) / obj_test.second;
    }*/
  }
  /*if (cost > 0.8)
    return cost;
  else*/
  return 0;
}

double neighborComparison(graphType & graph_train_splited, graphType & graph_test_splited,
  _List_iterator<_List_val<_List_simple_types<RelationNodeSubject<ActorLabel, ActorLabel, double>>>> ittr,
  _List_iterator<_List_val<_List_simple_types<RelationNodeSubject<ActorLabel, ActorLabel, double>>>> itte) {
  
  auto lim = 99999;
  int adj = 0, backto= 0;
  double cost = 0;

  if (lim < 0) {
  Neighbor:
      cost += degreeComparison(ittr->objectList_.size(), itte->objectList_.size());
 //-----------------------------------------------------------------------------
    for (auto obj_train : ittr->objectList_) {
      for (auto obj_test : itte->objectList_) {
        cost += adjacenciesComparison(obj_train, obj_test, adj);
      }
      if (adj == 0)
        cost += 1;
      adj = 0;
    }
    for (auto obj_test : itte->objectList_) {
      for (auto obj_train : ittr->objectList_) {
        if (obj_train.first.data_.id_ == obj_test.first.data_.id_)
          adj = 1;
      }
      if (adj == 0)
        cost += 1;
      adj = 0;
    }
          if (backto == 1)
          goto back1;
          else if (backto == 2)
          goto back2;
          else if (backto == 3)
          goto back3;
          else if (backto == 4)
          goto back4;
          else if (backto == 5)
          goto back5;
          else if (backto == 6)
          goto back6;
          else if (backto == 7)
          goto back7;
          }
  if (itte == graph_test_splited.listNodes_.begin()) {
    if (ittr == graph_train_splited.listNodes_.begin()) {
      if (graph_test_splited.listNodes_.size() == 1 && graph_train_splited.listNodes_.size() > 1) {
        ittr++;
        cost += ittr->objectList_.size() + 1;
        ittr--;
      }
      else if (graph_train_splited.listNodes_.size() == 1 && graph_test_splited.listNodes_.size() > 1) {
        itte++;
        cost += itte->objectList_.size() + 1;
        itte--;
      }
    }
    else if (++ittr == graph_train_splited.listNodes_.end()) {
      ittr--;
      if (graph_train_splited.listNodes_.size() > 1) {
        ittr--;
        cost += ittr->objectList_.size() + 1;
        ittr++;
      }
    }
    else {
      ittr--;
      if (graph_test_splited.listNodes_.size() == 1) {
        ittr--;
        cost += ittr->objectList_.size() + 1;
        ittr++; ittr++;
        cost += ittr->objectList_.size() + 1;
        ittr--;
      }
      else {
        ittr--;
        cost += ittr->objectList_.size() + 1;
        ittr++; ittr++; itte++;
        backto = 1;
        goto Neighbor;
      back1:
        ittr--; itte--;
      }
    }
  }
  else if (++itte == graph_test_splited.listNodes_.end()) {
    itte--;
    if (ittr == graph_train_splited.listNodes_.begin()) {
      if (graph_train_splited.listNodes_.size() > 1) {
        ittr++;
        cost += ittr->objectList_.size() + 1;
        ittr--;
      }
      itte--;
      cost += itte->objectList_.size() + 1;
      itte++;
    }
    else if (++ittr == graph_train_splited.listNodes_.end()) {
      ittr--;
      if (graph_train_splited.listNodes_.size() == 1) {
        itte--;
        cost += itte->objectList_.size() + 1;
        itte++;
      }
      else {
        itte--; ittr--;
        backto = 2;
        goto Neighbor;
      back2:
        itte++; ittr++;
      }
    }
    else {
      // Nao precisa porque passou pelo else if e ja incrementou i ittr ---> ittr++;
      cost += ittr->objectList_.size() + 1;
      ittr--; ittr--; itte--;
      backto = 3;
      goto Neighbor;
    back3:
      ittr++; itte++;
    }
  }
  else {
    itte--;
    if (ittr == graph_train_splited.listNodes_.begin()) {
      if (graph_train_splited.listNodes_.size() == 1) {
        itte--;
        cost += itte->objectList_.size() + 1;
        itte++; itte++;
        cost += itte->objectList_.size() + 1;
        itte--;
      }
      else {
        itte--;
        cost += itte->objectList_.size() + 1;
        itte++; itte++; ittr++;
        backto = 4;
        goto Neighbor;
      back4:
        itte--; ittr--;
      }
    }
    else if (++ittr == graph_train_splited.listNodes_.end()) {
      ittr--;
      itte++;
      cost += itte->objectList_.size() + 1;
      itte--; itte--; ittr--;
      backto = 5;
      goto Neighbor;
    back5:
      itte++; ittr++;
    }
    else {
      ittr--;
      ittr++; itte++;
      backto = 6;
      goto Neighbor;
    back6:
      ittr--; ittr--; itte--; itte--;
      backto = 7;
      goto Neighbor;
    back7:
      ittr++; itte++;
    }
  }
  return cost;
}


////////////////////////////////////////////////////////////////////////////////
double getDistance(Matrix<double> costMatrix,
                   int &use,
                   int numNodesTr,
                   int numNodesTes,
                   double &matchs) {

  Munkres<double> Hungarian;
  double cost = 0.0, percent = 0.0, dist = 0.0;
  
  Matrix<double> permMatrix(numNodesTr, numNodesTes);
  for (int i = 0; i < numNodesTr; i++) {
    for (int j = 0; j < numNodesTes; j++) {
      permMatrix(i, j) = costMatrix(i, j);
    }
  }
  
  Hungarian.solve(permMatrix);
  matchs = 0.0;

  for (int i = 0; i < numNodesTr; i++) {
    for (int j = 0; j < numNodesTes; j++) {
      if (permMatrix(i, j) == 0) {
        cost += costMatrix(i, j);
        if (costMatrix(i, j) < 1) {
          matchs += 1.0;
        }
        break;
      }
    }
  }
  
  if (numNodesTr >= numNodesTes) {
    percent = 1 - (matchs / numNodesTes);
      dist = (cost / numNodesTes) + (numNodesTr - numNodesTes) * percent;
  }
  else {
    percent = 1 - (matchs / numNodesTr);
    dist = (cost / numNodesTr) + (numNodesTes - numNodesTr) * percent;
  }
  return dist;
}

////////////////////////////////////////////////////////////////////////////////
graphType resume(Observed & src) {

  graphType newg;
  set<int> objects;
  int aux = 0;

  auto it = src.graph_.listNodes_.begin();
  auto Nodes = src.graph_;
  auto it2 = Nodes.listNodes_.begin();
  
  it2->objectList_.clear();
  for (auto &par : it->objectList_) {
    if (aux != par.first.data_.id_) {
      aux = par.first.data_.id_;
      it2->objectList_.push_back(par);
      objects.insert(par.first.data_.id_);
    }
  }
  auto  str = set2str(objects);
  newg.listNodes_.push_back(*it2);

  auto it3 = newg.listNodes_.begin();

  it++; it2++;
  for (; it != src.graph_.listNodes_.end(); ++it, it2++) {
    objects.clear(); it2->objectList_.clear();
    aux = 0;
    for (auto &par : it->objectList_) {
      if (aux != par.first.data_.id_) {
        aux = par.first.data_.id_;
        it2->objectList_.push_back(par);
        objects.insert(par.first.data_.id_);
      }
    }
    auto  str2 = set2str(objects);
    if (str != str2) {
      it--;
      it3->data_.end_ = it->data_.id_;
      it++;
      newg.listNodes_.push_back(*it2);
      it3++;
    }
    str = str2;
  }
  it--;
  it3->data_.end_ = it->data_.id_;
  it++;
  return newg;
}

////////////////////////////////////////////////////////////////////////////////
void AnomaliesSet(Observed& graph_test, set<AnomalyGt>& Anomalies_obs) {
  AnomalyGt Anom;
  int aux = 0, insert = 0;
 
  for (auto it = graph_test.graph_.listNodes_.begin(); it != graph_test.graph_.listNodes_.end(); it++) {
    if (it++ == graph_test.graph_.listNodes_.end() || graph_test.graph_.listNodes_.size() > 1) {
      it--;
      if (aux == 0) {
      if (it->data_.anomaly_ == true) {
        aux = 1;
        Anom.desc_ = "";
        Anom.id_ = it->data_.list_idx_;
        Anom.ini_ = it->data_.id_;
        Anom.fin_ = it->data_.end_;
        insert = 1;
      }
      else {
        if (insert)
          Anomalies_obs.insert(Anom);
        aux = 0;
        insert = 0;
      }
    }
      else {
        if (it->data_.anomaly_ == true) {
          Anom.desc_ = "";
          Anom.id_ = it->data_.list_idx_;
          Anom.fin_ = it->data_.end_;
        }
        else {
          if (insert)
            Anomalies_obs.insert(Anom);
          aux = 0;
          insert = 0;
        }
      }
    }
    else {
      it--;
      if (aux == 1) {
        if (it->data_.anomaly_ == true) {
          Anom.desc_ = "";
          Anom.id_ = it->data_.list_idx_;
          Anom.fin_ = it->data_.end_;
          Anomalies_obs.insert(Anom);
        }
      }
      else {
        if (it->data_.anomaly_ == true) {
          Anom.desc_ = "";
          Anom.id_ = it->data_.list_idx_;
          Anom.fin_ = it->data_.end_;
          Anom.ini_ = it->data_.id_;
          Anomalies_obs.insert(Anom);
        }
      }
    }
  }
}

static void anomaly1_vectorBuild(Observed &graph, set<AnomalyGt> &ans) {
  bool start = false;
  string desc = "";
  stack<AnomalyGt> cont;
  for (auto &node : graph.graph_.listNodes_) {
    //in case of anomalie creates or continues with the last anoamly
    if (node.data_.anomaly_) {
      //in case of anomalie does not been started
      if (!start) {
        cont.push(
          AnomalyGt(graph.id_, node.data_.id_, node.data_.id_, 1, desc)
        );
      }
      //when an anomaly was created before
      else {
        cont.top().fin_ = node.data_.id_;
      }
      start = true;
    }
    //in case of a normal node without anomaly
    else {
      if (!cont.empty()) {
        auto top = cont.top();
        ans.insert(top);
        cont.pop();
      }
      start = false;
    }
  }
}

/*else {
int loop;
while (parts < numNodesTes) {
cost = 0;
double match = 0;
if ((parts + numNodesTr) > numNodesTes) {
loop = parts;
parts = numNodesTes;
}
else {
loop = parts;
parts += numNodesTr;
}
Matrix<double> permMatrix(numNodesTr, (parts - loop));
for (int i = 0; i < numNodesTr; i++) {
for (int j = loop; j < parts; j++) {
permMatrix(i, (j - loop)) = costMatrix(i, j);
}
}
Hungarian.solve(permMatrix);
for (int i = 0; i < numNodesTr; i++) {
for (int j = loop; j < parts; j++) {
if (permMatrix(i, (j - loop)) == 0) {
cost += costMatrix(i, j);
use += 1;
if (costMatrix(i, j) < 1) {
match += 1.0;
}
break;
}
}
}
eq = match / (parts - loop);
if (eq > 0) {
percent = 1 - (match / (parts - loop));
dist1 += (cost / (parts - loop)) + (numNodesTr - (parts - loop)) * percent;
eq1 += eq;
}
else
dist2 += (cost / (parts - loop)) + (numNodesTr - (parts - loop));
matchs += match;
}
dist += (dist1 / (1 + eq1)) + dist2;
}*/




/*#include "Support2.h"
#include <iomanip>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void anomaly1_vectorBuild(Observed &graph, set<AnomalyGt> &ans);
bool testLevel3(  string & set_file, short frm_step,
                  map<string, int>  *objs,
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs,
                  set<int>          &obs,
                  int               anomalytype) {
  
  FileStorage fs(set_file, FileStorage::READ);
//..............................................................................
  string  directory,
          token,
          gt_dir,
          out_matching,
          out_val_file;
  
  double limit,
         prec,
         rec;
    
  fs["load_graph_token"]       >> token;
  fs["load_graph_directory"]   >> directory;
  fs["testing3_out_file"]      >> out_matching;
  fs["testing3_out_val_file"]  >> out_val_file;
  fs["testing3_gt_file"]       >> gt_dir;
  fs["testing3_treshold"]      >> limit;
  fs["testing3_prec"]          >> prec;
  fs["testing3_recall"]        >> rec;

  cutil_file_cont graph_files;
  list<Observed>  graphs_training;

  cout << "Loading training graphs...\n";
  list_files(graph_files, directory.c_str(), token.c_str());
  for (auto & file : graph_files) {
    cout << file << endl;
    loadDescribedGraphs(file, graphs_training, &obs);
  }

  set<AnomalyGt> Anomalies_obs,
                 Anomalies_gt;

  fstream        outFile, out_val;
  outFile.open(out_matching, ios::out);
  out_val.open(out_val_file, ios::out | ios::app);

  list<Observed> graphs_test_Af;

  vector<double> Maxdis;
//..............................................................................
  for (auto graph : graphs) {
    cout << "Processing test graph..." << endl;
    matchNodes(graph, graphs_training, anomalytype, limit, Maxdis);
    AnomaliesSet(graph, Anomalies_obs);
    graphs_test_Af.push_back(graph);
    for (auto iter = Anomalies_obs.begin(); iter != Anomalies_obs.end(); iter++) {
      outFile << "Anomalie Frame  " << iter->ini_ << " to Frame " << iter->fin_ << endl;
    }
  }
  cout << "Validating..." << endl;
  validateGraphList(graphs_test_Af, gt_dir, out_val_file,
    pair<double, double>(prec, rec), anomaly1_vectorBuild);
  out_val << Maxdis[0] << " " << Maxdis[1] << " " << Maxdis[2];
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
double matchNodes(Observed &graph_test, list<Observed> &graphs_training, int anomalytype, double limit, vector<double>& Maxdis) {
  
  for (auto it = graph_test.graph_.listNodes_.begin(); it != graph_test.graph_.listNodes_.end(); it++) {
    it->data_.end_ = it->data_.id_;
  }
  
  Maxdis.push_back(0); Maxdis.push_back(0); Maxdis.push_back(0);
  
  list<graphType> listGraphs_test, listGraphs_train;
  if (graph_test.graph_.listNodes_.size() > 2)
    splitGraph(graph_test, listGraphs_test, 3);
  else
    listGraphs_test.push_back(graph_test.graph_);

  double match_cost = 0.0, matchs = 0.0, distance = 0.0;
  int adj = 0, i = 0, Anom = 0, aux = 0, use = 0, backto = 0;
  
  //-----------------------------------------------------------------------------
  for (auto graph_test_splited : listGraphs_test) {
    vector<double> dist_values;
    int Anom = 1;
    for (auto graph_train : graphs_training) {
      for (auto it = graph_train.graph_.listNodes_.begin(); it != graph_train.graph_.listNodes_.end(); it++) {
        it->data_.end_ = it->data_.id_;
      }
      listGraphs_train.clear();
      if (graph_train.graph_.listNodes_.size() > 2)
        splitGraph(graph_train, listGraphs_train, 3);
      else
        listGraphs_train.push_back(graph_train.graph_);
//----------------------------------------------------------------------------- 
      for (auto graph_train_splited : listGraphs_train) {
        auto ittr = graph_train_splited.listNodes_.begin();
        int linha = 0;

        Matrix<double> costMatrix(graph_train_splited.listNodes_.size(),
          graph_test_splited.listNodes_.size());

        Munkres<double> Hung;

        for (auto node_train : graph_train_splited.listNodes_) {
          int coluna = 0;
          auto itte = graph_test_splited.listNodes_.begin();
          for (auto node_test : graph_test_splited.listNodes_) {
            match_cost = 0;
            if (itte == graph_test_splited.listNodes_.begin()) {

            }
            //-----------------------------Degree Comp-----------------------------------------------
            match_cost += degreeComparison(node_train.objectList_.size(), node_test.objectList_.size());
            //---------------------Adjacencies and Edge Comp-----------------------------------------------
            for (auto obj_train : node_train.objectList_) {
              for (auto obj_test : node_test.objectList_) {
                match_cost += adjacenciesComparison(obj_train, obj_test, adj);
              }
              if (adj == 0)
                match_cost += 1;
              adj = 0;
            }
            for (auto obj_test : node_test.objectList_) {
              for (auto obj_train : node_train.objectList_) {
                if (obj_train.first.data_.id_ == obj_test.first.data_.id_)
                  adj = 1;
              }
              if (adj == 0)
                match_cost += 1;
              adj = 0;
            }
            //----------------------------------Neighbor Comp------------------        
            match_cost = neighborComparison(graph_train_splited, graph_test_splited, ittr, itte);
            //---------------------------------End Neighbor Comp----------------
            costMatrix(linha, coluna) = match_cost;
            coluna++;
            itte++;
          }
          linha++;
          ittr++;
        }
        i++;
        distance = getDistance(costMatrix, use, graph_train_splited.listNodes_.size(),
                               graph_test_splited.listNodes_.size(), matchs);
        dist_values.push_back(distance);
        if (distance > limit) {
          if (Maxdis[0] < distance) {
            Maxdis[2] = Maxdis[1];
            Maxdis[1] = Maxdis[0];
            Maxdis[0] = distance;
          }
          else if (Maxdis[1] < distance) {
            Maxdis[2] = Maxdis[1];
            Maxdis[1] = distance;
          }
          else if (Maxdis[2] < distance)
            Maxdis[2] = distance;
        }
      }
    }
    for (int j = 0; j < dist_values.size(); j++) {
      if (dist_values[j] < limit) {
        Anom = 0;
        break;
      }
    }
    if (Anom == 1) {
      auto itsplit = graph_test_splited.listNodes_.begin();
      if (graph_test_splited.listNodes_.size() > 1) {
        itsplit++;
        for (auto itaux = graph_test.graph_.listNodes_.begin(); itaux != graph_test.graph_.listNodes_.end(); itaux++) {
          if (itaux->data_.id_ == itsplit->data_.id_)
            itaux->data_.anomaly_ = true;
          graph_test.levels_[anomalytype] = true;
        }
      }
    }
    Anom = 1;
  }
  return 1.0;
}


double degreeComparison(int train_node, int test_node) {
  if (train_node != test_node)
    return 1; //abs(train_node - test_node);
  return 0;
}

double adjacenciesComparison(pair<RelationNodeObject<ActorLabel>, double> & obj_train, pair<RelationNodeObject<ActorLabel>, double> &obj_test, int& adj) {
  if (obj_train.first.data_.id_ == obj_test.first.data_.id_) {
    adj = 1;
    if (obj_train.second > obj_test.second) {
      if (obj_train.second == 0)
        return abs(obj_train.second - obj_test.second) / 1;
      else
        return abs(obj_train.second - obj_test.second) / obj_train.second;
    }
    else {
      if (obj_test.second == 0)
        return abs(obj_test.second - obj_train.second) / 1;
      else
        return abs(obj_test.second - obj_train.second) / obj_test.second;
    }
  }
  return 0;
}

double neighborComparison(graphType & graph_train_splited, graphType & graph_test_splited,
                          _List_iterator<_List_val<_List_simple_types<RelationNodeSubject<ActorLabel, ActorLabel, double>>>> ittr,
                          _List_iterator<_List_val<_List_simple_types<RelationNodeSubject<ActorLabel, ActorLabel, double>>>> itte) {
  auto lim = 99999;
  int adj = 0, backto = 0;
  double cost = 0.0;

  if (lim < 0) {
  Neighbor:
    cost += degreeComparison(ittr->objectList_.size(), itte->objectList_.size());
    //-----------------------------------------------------------------------------
    for (auto obj_train : ittr->objectList_) {
      for (auto obj_test : itte->objectList_) {
        cost += adjacenciesComparison(obj_train, obj_test, adj);
      }
      if (adj == 0)
        cost += 1;
      adj = 0;
    }
    for (auto obj_test : itte->objectList_) {
      for (auto obj_train : ittr->objectList_) {
        if (obj_train.first.data_.id_ == obj_test.first.data_.id_)
          adj = 1;
      }
      if (adj == 0)
        cost += 1;
      adj = 0;
    }
    if (backto == 1)
      goto back1;
    else if (backto == 2)
      goto back2;
    else if (backto == 3)
      goto back3;
    else if (backto == 4)
      goto back4;
    else if (backto == 5)
      goto back5;
    else if (backto == 6)
      goto back6;
    else if (backto == 7)
      goto back7;
  }

  if (itte == graph_test_splited.listNodes_.begin()) {
    if (ittr == graph_train_splited.listNodes_.begin()) {
      if (graph_test_splited.listNodes_.size() == 1 && graph_train_splited.listNodes_.size() > 1) {
        ittr++;
        cost += ittr->objectList_.size() + 1;
        ittr--;
      }
      else if (graph_train_splited.listNodes_.size() == 1 && graph_test_splited.listNodes_.size() > 1) {
        itte++;
        cost += itte->objectList_.size() + 1;
        itte--;
      }
    }
    else if (++ittr == graph_train_splited.listNodes_.end()) {
      ittr--;
      if (graph_train_splited.listNodes_.size() > 1) {
        ittr--;
        cost += ittr->objectList_.size() + 1;
        ittr++;
      }
    }
    else {
      ittr--;
      if (graph_test_splited.listNodes_.size() == 1) {
        ittr--;
        cost += ittr->objectList_.size() + 1;
        ittr++; ittr++;
        cost += ittr->objectList_.size() + 1;
        ittr--;
      }
      else {
        ittr--;
        cost += ittr->objectList_.size() + 1;
        ittr++; ittr++; itte++;
        backto = 1;
        goto Neighbor;
      back1:
        ittr--; itte--;
      }
    }
  }
  else if (++itte == graph_test_splited.listNodes_.end()) {
    itte--;
    if (ittr == graph_train_splited.listNodes_.begin()) {
      if (graph_train_splited.listNodes_.size() > 1) {
        ittr++;
        cost += ittr->objectList_.size() + 1;
        ittr--;
      }
      itte--;
      cost += itte->objectList_.size() + 1;
      itte++;
    }
    else if (++ittr == graph_train_splited.listNodes_.end()) {
      ittr--;
      if (graph_train_splited.listNodes_.size() == 1) {
        itte--;
        cost += itte->objectList_.size() + 1;
        itte++;
      }
      else {
        itte--; ittr--;
        backto = 2;
        goto Neighbor;
      back2:
        itte++; ittr++;
      }
    }
    else {
      // Nao precisa porque passou pelo else if e ja incrementou i ittr ---> ittr++;
      cost += ittr->objectList_.size() + 1;
      ittr--; ittr--; itte--;
      backto = 3;
      goto Neighbor;
    back3:
      ittr++; itte++;
    }
  }
  else {
    itte--;
    if (ittr == graph_train_splited.listNodes_.begin()) {
      if (graph_train_splited.listNodes_.size() == 1) {
        itte--;
        cost += itte->objectList_.size() + 1;
        itte++; itte++;
        cost += itte->objectList_.size() + 1;
        itte--;
      }
      else {
        itte--;
        cost += itte->objectList_.size() + 1;
        itte++; itte++; ittr++;
        backto = 4;
        goto Neighbor;
      back4:
        itte--; ittr--;
      }
    }
    else if (++ittr == graph_train_splited.listNodes_.end()) {
      ittr--;
      itte++;
      cost += itte->objectList_.size() + 1;
      itte--; itte--; ittr--;
      backto = 5;
      goto Neighbor;
    back5:
      itte++; ittr++;
    }
    else {
      ittr--;
      ittr++; itte++;
      backto = 6;
      goto Neighbor;
    back6:
      ittr--; ittr--; itte--; itte--;
      backto = 7;
      goto Neighbor;
    back7:
      ittr++; itte++;

    }
  }
  return cost;
}



////////////////////////////////////////////////////////////////////////////////
double getDistance(Matrix<double> costMatrix,
                   int &use,
                   int numNodesTr,
                   int numNodesTes,
                   double &matchs) {

  Munkres<double> Hungarian;
  double cost = 0.0, percent = 0.0, dist = 0.0;
  
  Matrix<double> permMatrix(numNodesTr, numNodesTes);
  for (int i = 0; i < numNodesTr; i++) {
    for (int j = 0; j < numNodesTes; j++) {
      permMatrix(i, j) = costMatrix(i, j);
    }
  }
  
  Hungarian.solve(permMatrix);
  matchs = 0.0;

  for (int i = 0; i < numNodesTr; i++) {
    for (int j = 0; j < numNodesTes; j++) {
      if (permMatrix(i, j) == 0) {
        cost += costMatrix(i, j);
        if (costMatrix(i, j) < 1) {
          matchs += 1.0;
        }
        break;
      }
    }
  }
  
  if (numNodesTr >= numNodesTes) {
    percent = 1 - (matchs / numNodesTes);
      dist = (cost / numNodesTes) + (numNodesTr - numNodesTes) * percent;
  }
  else {
    percent = 1 - (matchs / numNodesTr);
    dist = (cost / numNodesTr) + (numNodesTes - numNodesTr) * percent;
  }
  return dist;
}

////////////////////////////////////////////////////////////////////////////////
graphType resume(Observed & src) {

  graphType newg;
  set<int> objects;
  int aux = 0;

  auto it = src.graph_.listNodes_.begin();
  auto Nodes = src.graph_;
  auto it2 = Nodes.listNodes_.begin();
  
  it2->objectList_.clear();
  for (auto &par : it->objectList_) {
    if (aux != par.first.data_.id_) {
      aux = par.first.data_.id_;
      it2->objectList_.push_back(par);
      objects.insert(par.first.data_.id_);
    }
  }
  auto  str = set2str(objects);
  newg.listNodes_.push_back(*it2);

  auto it3 = newg.listNodes_.begin();

  it++; it2++;
  for (; it != src.graph_.listNodes_.end(); ++it, it2++) {
    objects.clear(); it2->objectList_.clear();
    aux = 0;
    for (auto &par : it->objectList_) {
      if (aux != par.first.data_.id_) {
        aux = par.first.data_.id_;
        it2->objectList_.push_back(par);
        objects.insert(par.first.data_.id_);
      }
    }
    auto  str2 = set2str(objects);
    if (str != str2) {
      it--;
      it3->data_.end_ = it->data_.id_;
      it++;
      newg.listNodes_.push_back(*it2);
      it3++;
    }
    str = str2;
  }
  it--;
  it3->data_.end_ = it->data_.id_;
  it++;
  return newg;
}

////////////////////////////////////////////////////////////////////////////////
void AnomaliesSet(Observed& graph_test, set<AnomalyGt>& Anomalies_obs) {
  AnomalyGt Anom;
  int aux = 0, insert = 0;
 
  for (auto it = graph_test.graph_.listNodes_.begin(); it != graph_test.graph_.listNodes_.end(); it++) {
    if (it++ == graph_test.graph_.listNodes_.end() || graph_test.graph_.listNodes_.size() > 1) {
      it--;
      if (aux == 0) {
      if (it->data_.anomaly_ == true) {
        aux = 1;
        Anom.desc_ = "";
        Anom.id_ = it->data_.list_idx_;
        Anom.ini_ = it->data_.id_;
        Anom.fin_ = it->data_.end_;
        insert = 1;
      }
      else {
        if (insert)
          Anomalies_obs.insert(Anom);
        aux = 0;
        insert = 0;
      }
    }
      else {
        if (it->data_.anomaly_ == true) {
          Anom.desc_ = "";
          Anom.id_ = it->data_.list_idx_;
          Anom.fin_ = it->data_.end_;
        }
        else {
          if (insert)
            Anomalies_obs.insert(Anom);
          aux = 0;
          insert = 0;
        }
      }
    }
    else {
      it--;
      if (aux == 1) {
        if (it->data_.anomaly_ == true) {
          Anom.desc_ = "";
          Anom.id_ = it->data_.list_idx_;
          Anom.fin_ = it->data_.end_;
          Anomalies_obs.insert(Anom);
        }
      }
      else {
        if (it->data_.anomaly_ == true) {
          Anom.desc_ = "";
          Anom.id_ = it->data_.list_idx_;
          Anom.fin_ = it->data_.end_;
          Anom.ini_ = it->data_.id_;
          Anomalies_obs.insert(Anom);
        }
      }
    }
  }
}

static void anomaly1_vectorBuild(Observed &graph, set<AnomalyGt> &ans) {
  bool start = false;
  string desc = "";
  stack<AnomalyGt> cont;
  for (auto &node : graph.graph_.listNodes_) {
    //in case of anomalie creates or continues with the last anoamly
    if (node.data_.anomaly_) {
      //in case of anomalie does not been started
      if (!start) {
        cont.push(
          AnomalyGt(graph.id_, node.data_.id_, node.data_.id_, 1, desc)
        );
      }
      //when an anomaly was created before
      else {
        cont.top().fin_ = node.data_.id_;
      }
      start = true;
    }
    //in case of a normal node without anomaly
    else {
      if (!cont.empty()) {
        auto top = cont.top();
        ans.insert(top);
        cont.pop();
      }
      start = false;
    }
  }
}

/*else {
int loop;
while (parts < numNodesTes) {
cost = 0;
double match = 0;
if ((parts + numNodesTr) > numNodesTes) {
loop = parts;
parts = numNodesTes;
}
else {
loop = parts;
parts += numNodesTr;
}
Matrix<double> permMatrix(numNodesTr, (parts - loop));
for (int i = 0; i < numNodesTr; i++) {
for (int j = loop; j < parts; j++) {
permMatrix(i, (j - loop)) = costMatrix(i, j);
}
}
Hungarian.solve(permMatrix);
for (int i = 0; i < numNodesTr; i++) {
for (int j = loop; j < parts; j++) {
if (permMatrix(i, (j - loop)) == 0) {
cost += costMatrix(i, j);
use += 1;
if (costMatrix(i, j) < 1) {
match += 1.0;
}
break;
}
}
}
eq = match / (parts - loop);
if (eq > 0) {
percent = 1 - (match / (parts - loop));
dist1 += (cost / (parts - loop)) + (numNodesTr - (parts - loop)) * percent;
eq1 += eq;
}
else
dist2 += (cost / (parts - loop)) + (numNodesTr - (parts - loop));
matchs += match;
}
dist += (dist1 / (1 + eq1)) + dist2;
}*/
