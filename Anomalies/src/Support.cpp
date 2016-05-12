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
  string subject = "persona";
  //loading frame observation
  for (string line; getline(arc, line) && line.size() > 1;) {
    auto vline  = cutil_string_split(line);
    vector<int> vint(info_size);
    vint[0] = objs[subject];
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