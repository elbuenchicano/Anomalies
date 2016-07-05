#include "CUtil.h"

bool cmpStrNum(const std::string &a, const std::string &b)
{
  if (a.size() == b.size())return a < b;
  if (a.size() < b.size())return true;
  return false;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class t>
void ptr_mostrar(t a, int tam, std::ostream &os)
{
  for (int i = 0; i < tam; i++)
    os << a[i] << " ";
  os << endl;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class t>
static t num_dig(t num)
{
  int res = 0;
  for (; num > 0; ++res)
    num /= 10;
  return res;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::string insert_numbers(std::size_t innumber, std::size_t finaln, char token)
{
  std::stringstream digits;
  for (auto i = num_dig(finaln) - num_dig(innumber); i > 0; --i)
    digits << token;
  digits << innumber;
  return digits.str();
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> cutil_load2strv(std::string & file){
  std::ifstream arc(file);
  assert(arc.is_open());
  std::list<std::string> files;
  for (std::string line; std::getline(arc, line); )
    files.push_back(line);
  arc.close();
  std::vector<std::string> res(files.begin(), files.end());
  return res;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> cutil_string_split(std::string &str, char delim) {
  std::vector<std::string>  tokens;
  std::istringstream        iss(str);
  if (delim == ' ') {
    copy(std::istream_iterator<std::string>(iss),
      std::istream_iterator<std::string>(),
      std::back_inserter(tokens));
  }
  else {
    std::string item;
    while (getline(iss, item, delim))
      tokens.push_back(item);
  }
  return tokens;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::vector<cutil_grig_point> grid_generator(int w, int h, int cw, int ch, int ov_w, int ov_h)
{
  int wfin = w - cw + 1,
    hfin = h - ch + 1;
  std::vector<cutil_grig_point> res;
  for (int i = 0; i < wfin; i += ov_w)
  {
    for (int j = 0; j < hfin; j += ov_h)
    {
      cutil_grig_point cuboid{ i, j, (i + cw - 1), (j + ch - 1) };
      res.push_back(cuboid);
    }
  }
  return res;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream & os, cuboid_dim c)
{
  os << "p1 " << c.xi << " " << c.yi << std::endl;
  os << "p2 " << c.xf << " " << c.yf;
  return os;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static std::size_t lastToken(std::string &base, std::string token)
{
  std::size_t	posf = 0,
    posa = base.find(token);
  for (; posa != std::string::npos; posa = base.find(token, posa + 1))
    posf = posa;
  return posf;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::string cutil_GetLast(std::string & base, std::string token)
{
  return base.substr(0, lastToken(base, token));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::string cutil_invert(std::string & base)
{
  std::string res;
  res.resize(base.length());
  for (size_t i = 0; i < base.length(); ++i) {
    res[i] = base[i] == '/' ? '\\' : base[i];
  }
  return res;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::string cutil_LastName(std::string &base)
{
  auto  p1 = lastToken(base, "/"),
    p2 = lastToken(base, ".");
  return base.substr(p1 + 1, (p2 - p1) - 1);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::string cutil_antecessor(std::string & path, short step)
{
  std::string res = path;
  for (short i = 0; i < step; ++i)
  {
    auto pos = lastToken(res, "/");
    res = res.substr(0, pos);
  }
  return cutil_LastName(res);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::string cutil_string_trim(std::string str) {
  str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
  str.erase(str.find_last_not_of(' ') + 1);         //surfixing spaces
  return str;
}

std::string cutil_string_join(std::vector<std::string> & vline, int ex = 0) {
  std::string str = "";
  for (size_t i = 0; i < (vline.size() - ex); ++i)
    str += vline[i] + " ";
  return cutil_string_trim(str);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void cutil_create_new_dir_all(std::string & base)
{
  auto posa = base.find("/");
  for (posa = base.find("/", posa + 1); posa != std::string::npos;
  posa = base.find("/", posa + 1))
  {
    std::string dir = base.substr(0, posa);
    CreateDirectoryA(cutil_invert(dir).c_str(), NULL);
  }
  CreateDirectoryA(cutil_invert(base).c_str(), NULL);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//algorithms.................................................................




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          Dirent 
      
#ifdef DIRENT_H

void list_files(cutil_file_cont & sal, const char * d)
{
  sal.clear();
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(d)) != NULL) {
    while ((ent = readdir(dir)) != NULL)
    {
      sal.push_back(ent->d_name);
    }
    closedir(dir);
  }
  else
    perror("");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void list_files(cutil_file_cont & sal, const char *d, const char * token)
{
  sal.clear();
  DIR *dir;
  struct dirent *ent;
  std::string  path(d);
  path += "/";
  if ((dir = opendir(d)) != NULL) {
    while ((ent = readdir(dir)) != NULL)
    {
      std::string fil = ent->d_name;
      if (fil.find(token) + 1)
        sal.push_back(path + ent->d_name);
    }
    closedir(dir);
  }
  else
    perror("");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void list_files_all(cutil_file_cont & sal, const char *d, const char * token)
{
  DIR		*dir;
  struct	dirent *ent;
  std::string  path(d);
  path += "/";
  if ((dir = opendir(d)) != NULL) {
    while ((ent = readdir(dir)) != NULL)
    {
      std::string fil = ent->d_name;
      if (fil.find(token) + 1)
      {
        sal.push_back(path + ent->d_name);
      }
      else
      {
        if ((int)fil.find(".")<0)
          list_files_all(sal, std::string(path + ent->d_name).c_str(), token);
      }
    }
    closedir(dir);
  }
  else
    perror("");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void list_files_all(cutil_file_cont & sala, cutil_file_cont & salc, const char *d, const char * token)
{
  DIR		*dir;
  struct	dirent *ent;
  std::string  path(d);
  path += "/";
  if ((dir = opendir(d)) != NULL) {
    while ((ent = readdir(dir)) != NULL)
    {
      std::string fil = ent->d_name;
      if (fil.find(token) + 1) {
        sala.push_back(path + ent->d_name);
      }
      else {
        if ((int)fil.find(".") < 0) {
          salc.push_back(path + ent->d_name);
          list_files_all(sala, salc, std::string(path + ent->d_name).c_str(), token);
        }
      }
    }
    closedir(dir);
  }
  else
    perror("");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
std::ostream & operator << (std::ostream & os, DirectoryNode node)
{
  os << node._label << "-" << node._label << " ";
  return os;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void list_files_all_tree(DirectoryNode * node, const char * token)
{
  DIR		*dir;
  struct	dirent *ent;
  std::string  path = node->_label + "/";
  if ((dir = opendir(node->_label.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      std::string fil = ent->d_name;
      if (fil.find(token) + 1)
        node->_listFile.push_back(path + ent->d_name);
      else {
        if ((int)fil.find(".") < 0) {
          DirectoryNode * file = new DirectoryNode(path + ent->d_name, node);
          node->_sons.push_back(file);
          list_files_all_tree(*(node->_sons.end() - 1), token);
        }
      }
    }
    closedir(dir);
  }
  else
    perror("");//if someone wants to say something in erro cases XD
}

#endif
