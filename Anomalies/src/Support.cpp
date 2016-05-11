#include "Support.h"

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
void loadFrameItems(ifstream &arc, FrameItem &frame)
{
  for (string line; getline(arc, line) && line.size() > 1;) {
    auto vline = cutil_string_split(line);
    frame.sub_obj[vline[0] != "persona"].push_back(vline);
  }
}
////////////////////////////////////////////////////////////////////////////////
void loadFrameList(string file, list<FrameItem> & frameList, short frame_step) {
  ifstream seq(file);
  assert(seq.is_open());

  short nframe = 0;
  for (string line; getline(seq, line); ++nframe) {
    auto vline = cutil_string_split(line);
    if (vline.size() > 1) {
      if (vline[0] == "Frame_" && (nframe % frame_step == 0)) {
        FrameItem frm{ stoi(vline[1]) };
        loadFrameItems(seq, frm);
        frameList.push_back(frm);
      }
    }
  }

  seq.close();
}