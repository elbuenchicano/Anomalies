#include "Support.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadDefinitions(std::string file,  std::map<std::string, int> &info, 
                                        std::map<int, std::string> &infor){
  std::ifstream arc(file);
  for (std::string line; std::getline(arc, line); ) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      info[vline[0]] = stoi(vline[1]);
      infor[stoi(vline[1])] = vline[0];
    }
  }
  arc.close();
}
//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
static inline 
void string2intVec(vector<string> & vec, vector<int> &res,
                          size_t i, size_t f) {
  for (size_t it = i; it < f; ++it)
    res[it] = stoi(vec[it]);
}
//////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////
void loadFrameItems(ifstream &arc, FrameItem &frame, map<string, int> & objs)
{
  //containing the id and positions 
  //string for person
  string subject = "person";
  //loading frame observation
  for (string line; getline(arc, line) && line.size() > 1;) {
    auto vline  = cutil_string_split(line);
    vector<int> vint(vline.size());
    vint[0] = objs[vline[0]];
    string2intVec(vline, vint, 1, vint.size());
    frame.sub_obj[vline[0] != subject].push_back(vint);
  }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadFrameList( string file, list<FrameItem> & frameList, short frame_step,
                    map<string, int> & objs) {
  ifstream seq(file);
  assert(seq.is_open());

  short nframe = 0;
  for (string line; getline(seq, line); ++nframe) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      if (vline[0] == "Frame_" && (nframe % frame_step == 0)) {
        FrameItem frm{ stoi(vline[1]) };
        loadFrameItems(seq, frm, objs);
        frameList.push_back(frm);
      }
    }
  }

  seq.close();
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
  for (string line; getline(arc, line) && line.size() > 1;) {
    //cout << line << endl;
    auto vline  = cutil_string_split(line);
    switch (vline[0][0]) {
    case 'G': {
      graphsList.push_front(BaseDefinitions_tr::graphType());
      break;
    }
    case 'N': {
      auto head   = graphsList.begin();
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
