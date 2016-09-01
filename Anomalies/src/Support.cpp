#include "Support.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadDefinitions(std::string file,  std::map<std::string, int> &info, 
                                        std::map<int, std::string> &infor){
  std::ifstream arc(file);
  for (std::string line; std::getline(arc, line); ) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      auto name = cutil_string_join(vline, 1);
      int id = stoi(vline[vline.size()-1]);
        
      info[name]  = id;
      infor[id]   = name;
    }
  }
  arc.close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadDescribedGraphs( string file , 
                          list<Observed> & graphsList,
                          set<int> * observedObjs = nullptr)
{
  ifstream    arc(file);
  double      lastEdge;
  //...........................................................................
  assert(arc.is_open());
  for (string line; getline(arc, line);) {
    //cout << line << endl;
    if (line.size() > 1) {
      auto vline = cutil_string_split(line);
      switch (vline[0][0]) {
      case 'G': {
        graphsList.push_front(Observed(stoi(vline[1])));
        break;
      }
      case 'N': {
        auto head = graphsList.begin();
        head->graph_.addSubjectNode(ActorLabel(stoi(vline[1]), stoi(vline[2])));
        break;
      }
      case 'E': {
        lastEdge = stod(vline[1]);
        break;
      }
      case 'O': {
        auto head = graphsList.begin();
        head->graph_.addObjectRelation(ActorLabel(stoi(vline[1]), stoi(vline[2])), lastEdge);

        //adding the object to observed objects into the set
        if(observedObjs)
          observedObjs->insert(stoi(vline[1]));
        break;
      }
      }
    }
  }
  arc.close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void resumingGraphs( list<BaseDefinitions_tr::graphType> & src,
                     list<BaseDefinitions_tr::graphType> & dst) {

  dst.clear();
  for (auto & gr : src) {
    BaseDefinitions_tr::graphType newg;
    for (auto & graph: src) {
      

      auto it = graph.listNodes_.begin();
      set<int> objects;

      for (auto &par : it->objectList_) {
        objects.insert(par.first.data_.id_);
      }
      auto  str = set2str(objects);
      
      newg.listNodes_.push_back(*it);
      it++;
      for (;it != graph.listNodes_.end(); ++it) {
        
        for (auto &par : it->objectList_) {
          objects.insert(par.first.data_.id_);
        }
        auto  str2 = set2str(objects);
        if (str != str2) {
          newg.listNodes_.push_back(*it);
        }
        str = str2;
      }
    }
    dst.push_back(newg);

  }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

double distance2object( TrkPoint pt, TrkPoint sw, TrkPoint ne){

  //ask if pt is inside 
  if (sw.x < pt.x  && pt.x < ne.x && ne.y > pt.y  && pt.y > sw.y)
    return -1;
  //translate to origin
  double    theta;  //quadrant angle
  TrkPoint  center,
            nearest, 
            tmp;

  //............................................................................
  center.x = static_cast<float>((sw.x + ne.x) / 2.0);
  center.y = static_cast<float>((sw.y + ne.y) / 2.0);
  tmp     = pt - center;
  theta   = atan2( tmp.y , tmp.x ) * 180 / _PI;
  if (theta < 0) theta += 360;
  short  quad = static_cast<short>(theta/90);
  switch (quad) {
 
  case 0:{//first quadrant
    tmp = pt - ne;
    if (tmp.x < 0) return abs(pt.y - ne.y);
    if (tmp.y < 0) return abs(pt.x - ne.x);
    return norm(tmp);
  }
  
  case 1:{//second
    TrkPoint nw(sw.x, ne.y);
    tmp = pt - nw;
    if (tmp.x > 0) return abs(pt.y - nw.y);
    if (tmp.y < 0) return abs(pt.x - nw.x);
    return norm(tmp);
  }
  
  case 2:{//third
    tmp = pt - sw;
    if (tmp.x > 0) return abs(pt.y - sw.y);
    if (tmp.y > 0) return abs(pt.x - sw.x);
    return norm(tmp);
  }
  
  case 3:{//fourth
    TrkPoint se(ne.x, sw.y);
    tmp = pt - se;
    if (tmp.x < 0) return abs(pt.y - se.y);
    if (tmp.y > 0) return abs(pt.x - se.x);
    return norm(tmp);
  }
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void splitGraph(  Observed        &obs,
                  list<graphType> &splits,
                  short           tam){
  splits.clear();
  int cont;

  auto end = obs.graph_.listNodes_.end();
 
  for (int i = 0; i < tam; ++i) {
    auto it = obs.graph_.listNodes_.begin();
    //leaving the first 
    for (int j = 0; j < i; ++j)++it;

    cont = 0;
    //making tuples
    graphType tmp;
    for (auto it = obs.graph_.listNodes_.begin(); it != end; ++it) {
      if (cont++ % tam == 0 && tmp.listNodes_.size()>0) {
        splits.push_back(tmp);
        tmp.listNodes_.clear();
      }
      else {
        tmp.listNodes_.push_back(*it);
      }

    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//load described graph 
//set to string 
string set2str(set<int> & st) {
  stringstream ss;
  for (auto & it : st)
    ss << it << " ";
  return ss.str().substr(0,ss.str().size()-1);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Mat_<int>   graphHistrogram(  BaseDefinitions_tr::graphType & graph,
                              map<string, int>  & dist,
                              set<string> & voc) {
  Mat_<int> res(1, static_cast<int>(voc.size()));
  for (auto &node : graph.listNodes_) {
    set<int> objects;

    for (auto &par : node.objectList_) {
      objects.insert(par.first.data_.id_);
    }
    auto  str = set2str(objects);
    auto  it  = dist.find(str);
    if(it != dist.end())
      ++dist[str];
  }
  int cont = 0;
  for (auto & it : dist) {
    res(0, cont++) = it.second;
    it.second = 0;
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline string set_bin_vector(vector<bool> &bin, vector<int> &objs) {
  stringstream  ss;
   
  for (size_t i = 0; i < bin.size(); ++i) {
    if (bin[i])
      ss << objs[i] << " ";
  }
  return ss.str().substr(0, ss.str().size() - 1);
}

void distributionPermutation(set<int> &objs, map<string, int> & out) {

  
  vector<int> vobjs(objs.begin(), objs.end());
  int n     = static_cast<int>(objs.size());
  int per   = static_cast<int>(pow(2,n));
  vector<string>   permutations(per);
  string        str;
  vector<bool>  bin;

  for (int i = 0; i < per; ++i) {
    bin = algoUtil_int2boolVec(i,n);
    str = set_bin_vector(bin, vobjs);
    permutations[i] = str;
  }


  sort(permutations.begin(), permutations.end(), cmpStrNum);


  auto it = permutations.begin();
  for (size_t i = 0; i < permutations.size(); ++i, ++it) 
    out[*it] = static_cast<int>(i);
  
  /**/
}

//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
//THIS PART MAY HAVE THREADS
void executeFunctionVec(  vector<pf>  &vec, 
                          string      &cmd,
                          string      &settings_file,
                          short       frm_step, 
                          map<string, int> *objs, 
                          map<int, string> *objs_i,
                          graphLstT   &graphs){
  
  set<int>        obsObjs;
  auto objectives = cutil_string_split(cmd);
  loadGraphs(settings_file, graphs, obsObjs);
  
  for (auto & ob : objectives) {
    if (ob != "") {
      vec[stoi(ob)](  settings_file, frm_step, objs, objs_i,
                      graphs, obsObjs, stoi(ob));
    }
  }
}
//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
void dictionaryBuild( graphLstT &lst, string &out_file,
                      set<string> & voc) {

  voc.clear();

  for (auto &graph : lst) {
    for (auto &node : graph.graph_.listNodes_) {
      set<int> objects;

      for (auto &par : node.objectList_) {
        objects.insert(par.first.data_.id_);
      }
      auto  str = set2str(objects);
      if (str != "")
        voc.insert(str);
    }
  }
  ofstream arc(out_file);
  cout << "Written in " << out_file << endl;
  cutil_cont2os(arc, voc, "\n");
  arc.close();
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void listObservedObjs(graphLstT &lst, string &out_file,
  set<int> &voc) {
  voc.clear();

  for (auto &graph : lst)
    for (auto &node : graph.graph_.listNodes_)
      for (auto &par : node.objectList_)
        voc.insert(par.first.data_.id_);
  ofstream arc(out_file);
  cout << "Written in " << out_file << endl;
  cutil_cont2os(arc, voc, "\n");
  arc.close();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void distributions(graphLstT &lst, string &out_file,
  set<string> & voc) {
  map <string, int> dist;

  //initalizing with zero values
  for (auto & it : voc)
    dist[it] = 0;

  Mat_<int> histograms;
  //creating for each graph
  for (auto &graph : lst) {
    auto h = graphHistrogram(graph.graph_, dist, voc);
    histograms.push_back(h);
  }

  FileStorage fs(out_file, FileStorage::WRITE);
  cout << "Written in " << out_file << endl;
  fs << "Histograms" << histograms;


}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void distributionBuild(graphLstT   &lst, string &out_file,
  set<int>    &obsO,
  map<string, int> & voc) {

  voc.clear();
  distributionPermutation(obsO, voc);

  Mat_<int> histograms;
  for (auto &graph : lst) {
    auto h = distribution(graph.graph_, voc);
    histograms.push_back(h);
  }

  //saving to file 

  FileStorage fs(out_file, FileStorage::WRITE);
  cout << "Written in " << out_file << endl;
  fs << "Histograms" << histograms;
}

Mat_<int> distribution( graphType         &graph,
                        map<string, int>  &dist) {

  Mat_<int> histo(1, static_cast<int>(dist.size()));
  histo = histo * 0;
  vector<string> tmp;

  for (auto &node : graph.listNodes_) {
    set<int> objects;

    for (auto &par : node.objectList_) {
      objects.insert(par.first.data_.id_);
    }
    auto  str = set2str(objects);
    tmp = cutil_string_split(str);

    if (tmp.size() > 1) {
      for (auto &it : tmp) {
        auto pt = dist.find(it);
        if(pt!=dist.end())
          ++histo(0, dist[it]);
      }
    }
    auto pt = dist.find(str);
    if (pt != dist.end())
      ++histo(0, dist[str]);
  }

  return histo;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool loadGraphs(  string          &set_file,
                  list<Observed>  &graph_list,
                  set<int>        &observedObjs) {
  
  int flag;
  FileStorage fs(set_file, FileStorage::READ);
  
  //............................................................................
  fs["load_graph_flag"] >> flag;
  if (!flag) {
    string  directory,
            token;

    cutil_file_cont graph_files;

    fs["load_graph_token"]      >> token;
    fs["load_graph_directory"]  >> directory;

    cout << "Loading graphs...\n";
    list_files(graph_files, directory.c_str(), token.c_str());
    for (auto & file : graph_files) {
      cout << file << endl;
      loadDescribedGraphs(file, graph_list, &observedObjs);
    }
  }
  else {
    string  file;
    fs["load_graph_file"] >> file;
    if (file != "") {
      cout << "Loading graph: ";
      cout << file << endl;
      loadDescribedGraphs(file, graph_list, &observedObjs);
    }
    else{
      cout << "Not found file" << endl;
      return false;
    }
  }
  return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool trainLevel1( string & set_file, short frm_step,
                  map<string, int>  *objs, 
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs, 
                  set<int>          &obs,
                  int               anomalytype) {

  cout << "\nTrain level 1 executing...\n";
  string  out_voc_file,
          out_dist_file;
  FileStorage fs(set_file, FileStorage::READ);
  fs["train1_out_voc_file"]   >> out_voc_file;
  fs["train1_out_dist_file"]  >> out_dist_file;

  //............................................................................
  //saving vocabulary in file
  set<string> voc;
  dictionaryBuild(graphs, out_voc_file, voc);

  //............................................................................
  //recording the distributions
  distributions(graphs, out_dist_file, voc);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool trainLevel2( string & set_file, short frm_step,
                  map<string, int>  *objs,
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs,
                  set<int>          &obs,
                  int               anomalytype) {
  cout << "\nTrain level 2 executing...\n";

  string  out_voc_file,
          out_obs_file,
          out_dist_file;
  FileStorage fs(set_file, FileStorage::READ);
  fs["train2_out_voc_file"]   >> out_voc_file;
  fs["train2_out_obs_file"]   >> out_obs_file;
  fs["train2_out_dist_file"]  >> out_dist_file;

  //............................................................................
  //saving the observed objects
  ofstream arc(out_obs_file);
  cout << "Observed ojects saved in: " << out_obs_file << endl;
  cutil_cont2os(arc, obs, "\n");
  arc.close();

  map<string, int> voc;
  distributionBuild(graphs, out_dist_file, obs, voc);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool testLevel1(  string & set_file, short frm_step,
                  map<string, int>  *objs,
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs,
                  set<int>          &obs,
                  int               anomalytype) {
  cout << "\nTest level 1 executing...\n";

  string  voc_file,
          out_file,
          gt_file;

  int     pos;

  FileStorage fs(set_file, FileStorage::READ);
  //............................................................................
  fs["testing1_voc_file"] >> voc_file;
  fs["testing1_out_file"] >> out_file;
  fs["testing1_gt_file"]  >> gt_file;

  //............................................................................
  //first level anomalie recognition 
  stringstream ss;
  auto voc = cutil_load2strv(voc_file);
  for (auto &graph : graphs) {
    for (auto &node : graph.graph_.listNodes_) {
      set<int> objects;
      for (auto &par : node.objectList_)
        objects.insert(par.first.data_.id_);
      auto  str = set2str(objects);

      if (!algoUtil_bin_search<string>(voc, str, pos)) {
        ss << "Warning: G-" << graph.id_ << "\t";
        ss << "Unknown word: " << str << ", In frame:" << node.data_.id_ <<"\n";
        node.data_.anomaly_         = true;
        graph.levels_[anomalytype]  = true;
      }
    }
  }
  cout << ss.str();
  ofstream arc(out_file);
  arc << ss.str();
  arc.close();

  //TO DO: present in images 

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool testLevel2(  string & set_file, short frm_step,
                  map<string, int>  *objs,
                  map<int, string>  *objs_i,
                  list<Observed>    &graphs,
                  set<int>          &obs, 
                  int               anomalytype) {
  cout << "\nTest level 2 executing...\n";

  string  histo_file,
          obs_file,
          out_file;

  FileStorage fs(set_file, FileStorage::READ);
    
  Mat_<int> histos;

  double    thr;

  //............................................................................

  fs["testing2_histo_file"] >> histo_file;
  fs["testing2_out_file"]   >> out_file;
  fs["testing2_obs_file"]   >> obs_file;
  fs["testing2_thr"]        >> thr;
  
  //............................................................................
  FileStorage fs2(histo_file, FileStorage::READ);
  fs2["Histograms"] >> histos;
  
  //............................................................................
  //second level anomalie recognition
  auto vstr = cutil_load2strv(obs_file);
  set<int> obsO;
  for (auto &it : vstr)
    obsO.insert(stoi(it));
  
  //............................................................................
  //second level anomalie recognition
  map<string, int> voc;
  distributionPermutation(obsO, voc);

  Mat_<int>     line;

  double        dist,
                min = FLT_MAX;

  stringstream  wrg;

  list<bool>    anomalyList;

  for (auto &graph : graphs) {
    auto h = distribution(graph.graph_, voc);
    for (int i = 0; i < histos.rows; ++i) {
      line = histos.row(i);
      dist = norm(h, line);
      if (dist < min) min = dist;
    }
    if (dist > thr) {
      wrg << "Warning Graph!" << graph.graph_.listNodes_.begin()->data_.id_ << endl;
      anomalyList.push_back(true);
    }
    else
      anomalyList.push_back(false);
  }

  //............................................................................
  //savin in output file
  cout << wrg.str();

  if (out_file != "") {
    ofstream arc(out_file);
    cout << "Warnings saved in: " << out_file << endl;
    arc << wrg.str();
    arc.close();
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadGt(string &file, set<AnomalyGt> &anomalies) {
  ifstream arc(file);
  assert(arc.is_open());
  for (string line; getline(arc, line);) {
    auto vline = cutil_string_split(line);
    anomalies.insert(AnomalyGt
        ( stoi(vline[0]),
          stoi(vline[1]),
          stoi(vline[2]),
          stoi(vline[3]),
          vline[4]
         )
      );
  }
  //copy(tmp.begin(), tmp.end(), back_inserter(anomalies));
  arc.close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//given a graph this function creates the anomaly vector to be compared & shown
//in this specific case is for anomaly type 1, epecific objects does not be 
//seen before in training stage
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
      else{
        cont.top().fin_ = node.data_.id_;
      }
      start = true;
    }
    //in case of a normal node without anomaly
    else {
      if (!cont.empty()) {
        auto top = cont.top();
        ans.insert(top);
      }
      start = false;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline static 
double jacccard_index(const AnomalyGt &A, const AnomalyGt &B) {

  double  left_int  = A.ini_, 
          left_ext  = B.ini_, 
          right_int = A.fin_,
          right_ext = B.fin_;

  if (left_int < left_ext)
    swap(left_ext, left_int);

  if (right_ext < right_int)
    swap(right_ext, right_int);

  double  A_n_B = left_int - right_int;
  double  A_u_B = left_ext + right_ext;

  return  A_n_B < 0 ? 0 : A_n_B / A_u_B;

}

void computeMetrics(  set<AnomalyGt> &    gt,
                      set<AnomalyGt> &    ans,
                      Metric_units   &    metrics,
                      double              jaccard_th
) {
  double max_jacc = 0;
  vector<bool> checked(ans.size());
  for (auto &an : gt) {
    size_t  choose  = -1, 
            pos     = 0;
    for (auto it = ans.begin(); it != ans.end() &&
                                an.fin_ >= it->ini_ ; ++it) {
      auto jacc = jacccard_index(an, *it);
      if (jacc > max_jacc) {
        max_jacc = jacc;
        choose = pos;
      }
      ++pos;
    }
    if (max_jacc >= jaccard_th) {
      //metrics.
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void validateGraphList( list<Observed>  &graphs, 
                        string          &gt_file, 
                        string          &out_file, 
                        int             atype,
                        double          jaccard_th,
                        void(*vectorBuild)(
                          Observed       &,
                          set<AnomalyGt> &)
  ){
  
  set<AnomalyGt>  anomalies,
                  anom_obs;

  //............................................................................
  loadGt(gt_file, anomalies);
  
  for (auto &graph : graphs) 
    vectorBuild(graph, anom_obs);
  
  

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////OLD FUNCTIONS


//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
static inline
void string2intVec(vector<string> & vec, vector<int> &res,
  size_t i, size_t f) {
  for (size_t it = i, cont = 1; it < f; ++it, ++cont)
    res[cont] = stoi(vec[it]);
}
//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
void loadFrameItems(vector<string >&vline, list<FrameItem>::iterator &frame,
  map<string, int> & objs)
{
  //containing the id and positions 
  //string for person
  string subject = "person";
  size_t i, j;
  vector<int> vint;
  if (vline.size() == 10) {
    vint.resize(vline.size());
    vint[0] = objs[vline[0]];
    string2intVec(vline, vint, 1, vint.size());
  }
  else {
    j = vline.size() - 6;
    for (i = 1; i <= j; i++) {
      vline[0] += ' ' + vline[i];
    }
    vint.resize(6);
    string2intVec(vline, vint, j + 1, vint.size());
    vint[0] = objs[vline[0]];
  }

  vint[3] += vint[1];
  vint[4] += vint[2];
  frame->sub_obj[vline[0] != subject].push_back(vint);

}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadFrameList(string file, list<FrameItem> & frameList, short frame_step,
  map<string, int> & objs) {
  ifstream seq(file);
  assert(seq.is_open());
  list<FrameItem> tmp;
  short nframe = 0;
  for (string line; getline(seq, line); ) {
    //cout << line << endl;
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      if (vline[0] == "Frame") {
        FrameItem frm{ stoi(vline[1]) };
        tmp.push_back(frm);
      }
      else {
        auto it = tmp.end();
        it--;
        loadFrameItems(vline, it, objs);
      }
    }
  }
  seq.close();
  int cont = 0;
  frameList.clear();
  for (auto & item : tmp) {
    if ((cont++ % frame_step) == 0)
      frameList.push_back(item);
  }


}






