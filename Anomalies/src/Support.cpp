#include "Support.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void loadDefinitions(std::string file, std::map<std::string, int> &info) {
  std::ifstream arc(file);
  for (std::string line; std::getline(arc, line); ) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1)
      info[vline[0]] = stoi(vline[1]);
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
  size_t info_size = 5;
  //string for person
  string subject = "person";
  //loading frame observation
  for (string line; getline(arc, line) && line.size() > 1;) {
    auto vline  = cutil_string_split(line);
    vector<int> vint(info_size);
    vint[0] = objs[vline[0]];
    string2intVec(vline, vint, 1, info_size);
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
void loadDescribedGraphs(string file , list<BaseDefinitions_tr::graphType>& graphsList)
{
  ofstream arc(file);

  arc.close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static double distance2object_sup(TrkPoint pt, TrkPoint center, TrkPoint no, 
                                  TrkPoint se){

  //ask if pt is inside 
  if (no.x < pt.x  && pt.x < se.x && no.y < pt.y  && pt.y < se.y)
    return -1;
  //translate to origin
  double    theta; //angle
  TrkPoint  nearest;
  pt    = pt - center;
  theta = tan( pt.y/pt.x );
  if (theta < 0)
  {
    if( pt.y > 0 )
    nearest =  ? no : se;
    pt = pt - nearest;
    theta =  tan(pt.y / pt.x);
    return ( (theta < 0)? 
      norm(nearest - pt) : 
      abs((nearest.x < 0)?
          
        )  
      );
  }
  else
    nearest = pt.x > 0 ? TrkPoint(no.x,se.y) : TrkPoint(se.x,no.y);
  
}

double distance2object(TrkPoint el1, TrkPoint el2, TrkPoint no, TrkPoint se)
{
  TrkPoint  center ( static_cast<float>((no.x + se.x) / 2.0),
                     static_cast<float>((no.y + se.y) / 2.0));



  //auto dist = cv::norm(prd - subCenter);

}


