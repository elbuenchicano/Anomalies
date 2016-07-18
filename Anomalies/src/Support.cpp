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
static inline 
void string2intVec( vector<string> & vec, vector<int> &res,
                    size_t i, size_t f) {
  for (size_t it = i, cont = 1; it < f; ++it, ++cont)
    res[cont] = stoi(vec[it]);
}
//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
void loadFrameItems(  vector<string >&vline, list<FrameItem>::iterator &frame, 
                      map<string, int> & objs)
{
  //containing the id and positions 
  //string for person
  string subject = "person";
  int i, j;
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
void loadFrameList( string file, list<FrameItem> & frameList, short frame_step,
                    map<string, int> & objs) {
  ifstream seq(file);
  assert(seq.is_open());
  list<FrameItem> tmp;
  short nframe = 0;
  for (string line; getline(seq, line); ) {
    //cout << line << endl;
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      if (vline[0] == "Frame" ) {
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadDescribedGraphs( string file , 
                          list<BaseDefinitions_tr::graphType>& graphsList)
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
        graphsList.push_front(BaseDefinitions_tr::graphType());
        break;
      }
      case 'N': {
        auto head = graphsList.begin();
        head->addSubjectNode(ActorLabel(stoi(vline[1]), stoi(vline[2])));
        break;
      }
      case 'E': {
        lastEdge = stod(vline[1]);
        break;
      }
      case 'O': {
        auto head = graphsList.begin();
        head->addObjectRelation(ActorLabel(stoi(vline[1]), stoi(vline[2])), lastEdge);
        break;
      }
      }
    }
  }
  arc.close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

double distance2object( TrkPoint pt, TrkPoint nw, TrkPoint se){

  //ask if pt is inside 
  if (nw.x < pt.x  && pt.x < se.x && nw.y > pt.y  && pt.y > se.y)
    return -1;
  //translate to origin
  double    theta;  //quadrant angle
  TrkPoint  center,
            nearest, 
            tmp;

  //............................................................................
  center.x = static_cast<float>((nw.x + se.x) / 2.0);
  center.y = static_cast<float>((nw.y + se.y) / 2.0);
  tmp     = pt - center;
  theta   = atan2( tmp.y , tmp.x ) * 180 / _PI;
  if (theta < 0) theta += 360;
  short  quad = static_cast<short>(theta/90);
  switch (quad) {
 
  case 0:{//first quadrant
    TrkPoint ne(se.x, nw.y);
    tmp = pt - ne;
    if (tmp.x < 0) return abs(pt.y - ne.y);
    if (tmp.y < 0) return abs(pt.x - ne.x);
    return norm(tmp);
  }
  
  case 1:{//second
    tmp = pt - nw;
    if (tmp.x > 0) return abs(pt.y - nw.y);
    if (tmp.y < 0) return abs(pt.x - nw.x);
    return norm(tmp);
  }
  
  case 2:{//third
    TrkPoint sw(nw.x, se.y);
    tmp = pt - sw;
    if (tmp.x > 0) return abs(pt.y - sw.y);
    if (tmp.y > 0) return abs(pt.x - sw.x);
    return norm(tmp);
  }
  
  case 3:{//fourth
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
////////////////////////////////////////////////////////////////////////////////
//load described graph 
//set to string 
string set2str(set<int> & st) {
  stringstream ss;
  for (auto & it : st)
    ss << it;
  return ss.str();
}
