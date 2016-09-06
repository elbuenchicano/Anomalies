#include "Support2.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool testLevel3(  string & set_file, short frm_step,
                  map<string, int>  *objs,
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs,
                  set<int>          &obs,
                  int) {
  
  FileStorage fs(set_file, FileStorage::READ);
//..............................................................................
  string  directory,
          token,
          gt_dir,
          out_matching;
  
  fs["load_graph_token"] >> token;
  fs["load_graph_directory"] >> directory;
  fs["out_matching_dir"] >> out_matching;
  fs["groundtruth_dir"] >> gt_dir;

  cutil_file_cont graph_files;
  list<Observed>  graphs_training;

  cout << "Loading training graphs...\n";
  list_files(graph_files, directory.c_str(), token.c_str());
  for (auto & file : graph_files) {
    cout << file << endl;
    loadDescribedGraphs(file, graphs_training, &obs);
  }

//..............................................................................
  for (auto graph : graphs) {
    matchNodes(graph, graphs_training, out_matching);
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////////
double matchNodes(Observed &graph_test, list<Observed> &graphs_training, string out_matching) {
//  graph_test.graph_ = resume(graph_test);

  double match_cost = 0.0, matchs = 0.0, distance = 0.0;
  int adj = 0, i = 0, Anom = 0, aux = 0, use = 0;

  fstream outFile;
  outFile.open(out_matching, ios::out | ios::app);  
  outFile << "Graph " << graph_test.id_ << endl;

  for (auto graph_train : graphs_training) {
  //  graph_train.graph_ = resume(graph_train);
    int linha = 0;

    Matrix<double> costMatrix(graph_train.graph_.listNodes_.size(),
                              graph_test.graph_.listNodes_.size());

    Munkres<double> Hung;

    for (auto node_train : graph_train.graph_.listNodes_) {
      int coluna = 0;
      for (auto node_test : graph_test.graph_.listNodes_) {
        match_cost = 0;
        if (node_train.objectList_.size() != node_test.objectList_.size())
          match_cost += 1;
        /*for (auto obj_train : node_train.objectList_) {
          auto iter = find(node_test.objectList_.begin(), node_test.objectList_.end(),
            obj_train);
          if (iter == node_test.objectList_.end())
            match_cost += 1;
          else {
            double Edge;
            if (obj_train.second > iter->second)
              match_cost = abs(obj_train.second - iter->second) / obj_train.second;
            else
              match_cost = abs(obj_train.second - iter->second) / iter->second;
          }
        }
        for (auto obj_test : node_test.objectList_) {
          auto iter = find(node_train.objectList_.begin(), node_train.objectList_.end(),
            obj_test);
          if (iter == node_train.objectList_.end())
            match_cost += 1;
        }*/
//-----------------------------------------------------------------------------
        for (auto obj_train : node_train.objectList_) {
          for (auto obj_test : node_test.objectList_) {
            if (obj_train.first.data_.id_ == obj_test.first.data_.id_) {
              adj = 1;
              if (obj_train.second > obj_test.second)
                if(obj_train.second == 0)
                  match_cost += abs(obj_train.second - obj_test.second)/1;
                else
                  match_cost += abs(obj_train.second - obj_test.second) / obj_train.second;
              else
                if(obj_test.second == 0)
                  match_cost += abs(obj_test.second - obj_train.second) / 1;
                else
                  match_cost += abs(obj_test.second - obj_train.second)/ obj_test.second;
            }
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
//------------------------------------------------------------------------------        
        costMatrix(linha, coluna) = match_cost;
        coluna++;
        cout << endl << endl;
      }
      linha++;
    }
    i++;
    distance = getDistance(costMatrix, use, graph_train.graph_.listNodes_.size(),
                           graph_test.graph_.listNodes_.size(), matchs);
    outFile << "Distance Test Graph " << graph_test.id_ << " to Graph Training "
            << i << " = " << distance << endl;
    if (Anom > distance)
      Anom = distance;
  }
  if (Anom > 1) {}
    
  outFile << endl << endl;
  return 1.0;
}

////////////////////////////////////////////////////////////////////////////////
double getDistance(Matrix<double> costMatrix, 
                   int &use,
                   int numNodesTr, 
                   int numNodesTes, 
                   double &matchs) {
  
  Munkres<double> Hungarian;
  int parts = 0;
  double cost = 0.0, percent = 0.0, dist = 0.0, dist1 = 0.0, dist2 = 0.0, eq = 0,
         eq1 = 0;
  matchs = 0.0; use = 0;;
  if (numNodesTr > numNodesTes) {
    Matrix<double> permMatrix(numNodesTr, numNodesTes);
    for (int i = 0; i < numNodesTr; i++) {
      for (int j = 0; j < numNodesTes; j++) {
        permMatrix(i, j) = costMatrix(i, j);
      }
    }
    Hungarian.solve(permMatrix);
    for (int i = 0; i < numNodesTr; i++) {
      for (int j = 0; j < numNodesTes; j++) {
        if (permMatrix(i, j) == 0) {
          cost += costMatrix(i, j);
          use += 1;
          if (costMatrix(i, j) < 1) {
            matchs += 1.0;
          }
          break;
        }
      }
    }
    percent = 1 - (matchs / numNodesTes);
    dist = (cost / numNodesTes) + (numNodesTr - numNodesTes) * percent;
  }
  else {
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
  }
  return dist;
}

////////////////////////////////////////////////////////////////////////////////
graphType resume(Observed & src) {

  graphType newg;

  auto it = src.graph_.listNodes_.begin();
  set<int> objects;
  for (auto &par : it->objectList_) {
    objects.insert(par.first.data_.id_);
      }
    auto  str = set2str(objects);

    newg.listNodes_.push_back(*it);
    it++;
    for (; it != src.graph_.listNodes_.end(); ++it) {
      objects.clear();
      for (auto &par : it->objectList_) {
        objects.insert(par.first.data_.id_);
      }
      auto  str2 = set2str(objects);
      if (str != str2) {
        newg.listNodes_.push_back(*it);
      }
      str = str2;
  }
    return newg;
}
