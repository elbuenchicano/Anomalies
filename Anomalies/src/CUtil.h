#ifndef CUTIL_H
#define CUTIL_H
#define NOMINMAX

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "dirent.h"
#include <math.h>
#include <iterator>
#include <list>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <algorithm>
#include <map>
#include <memory>
#include <stack>
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//Cutil is a utilitaries c++ functions for strings and directories............
//----------------------------------------------------------------------------


//-----------------Definciones-----------------------------------------------
//----------------------------------------------------------------------------

typedef unsigned int	uint;


//-----------------variables--------------------------------------------------
//----------------------------------------------------------------------------

///////////////////////////FUNCIONES//////////////////////////////////////////
//funcion de comparacion por orden numerico
bool cmpStrNum(const std::string &a, const std::string &b);
//---------------------------------------------------------------------------
//mostrar cutil_file_contenido de un puntero 
template <class t>
void ptr_mostrar(t a, int tam, std::ostream &os = std::cout);

//save std container to file 
template <class t> void cutil_cont2os(std::ostream &os, t &con, std::string sep) {
  for (auto & it : con)
    os << it << sep;
}

//load txt file to vector<string> 
std::vector<std::string> cutil_load2strv(std::string &);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//insert zeros into the front to get better renaming
//---------------------------------------------------------------------------

template <class t> static t num_dig(t num);

std::string insert_numbers(std::size_t innumber, std::size_t finaln, char token = '0');

std::vector<std::string> cutil_string_split(std::string &str, char delim = ' ');

//returns the string after the last token i.e c:/a/b returns b
std::string cutil_GetLast(std::string & base, std::string token);

//inverts the \ to /
std::string cutil_invert(std::string & base);

//returns the name of file witout extension
std::string cutil_LastName(std::string &base);

//returns the path step is the antecesor number
std::string cutil_antecessor(std::string & path, short step);

//create direcotries 
void cutil_create_new_dir_all(std::string & base);

//trim espaces in string 
std::string cutil_string_trim(std::string);

//join string vector in string
std::string cutil_string_join(std::vector<std::string> &, int);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//algorithms.................................................................






/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//-----------------grig generator--------------------------------------------
struct cuboid_dim {
  int xi, yi, xf, yf;
};
typedef cuboid_dim cutil_grig_point;
//w = frame width 
//h = frame height  
//cw = cuboid width
//ch = cuboid height
//ov_w = overlap_width
//ov_h = overlap_height
std::vector<cutil_grig_point> grid_generator(int w, int h, int cw, int ch, int ov_w, int ov_h);
//
std::ostream & operator << (std::ostream & os, cuboid_dim c);

/////////////////////////////////////////////////////////////////////////////
//-----------------Parte del dirent------------------------------------------
#ifdef DIRENT_H
//-----------------------------------------------------

typedef std::vector<std::string>	cutil_file_cont;// estructura donde se guarda el resultado STD!!
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
///Lista todos los archivos de una carpeta incluyendo ..                                                  
void list_files(cutil_file_cont & sal, const char * d);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
///Lista todos los archivos con el token de una carpeta 
void list_files(cutil_file_cont & sal, const char *d, const char * token);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
///Lista todos los archivos token de una carpeta y subcarpetas
void list_files_all(cutil_file_cont & sal, const char *d, const char * token);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
///Lista todos los archivos token de una carpeta y subcarpetas ademas de 
///listar tambien todas las subcarpetas
void list_files_all(cutil_file_cont & sala, cutil_file_cont & salc, const char *d, const char * token);
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
//Data estructure that contains the file tree of a directory____________________
struct	DirectoryNode;
typedef std::vector<DirectoryNode*>	SonsType;
//Filelistype may be a pointer to reduce the memory space but is not much.......
typedef std::vector<std::string>	FileListType;

//______________________________________________________________________________
//Directory node simulates directory in the SO..................................
struct DirectoryNode {
  std::string		_label;
  SonsType		_sons;
  FileListType	_listFile;
  DirectoryNode	*_father = nullptr;
  DirectoryNode(std::string label, DirectoryNode * father) :
    _label(label),
    _father(father) {}
  DirectoryNode(std::string label) :
    _label(label) {}
};
std::ostream & operator << (std::ostream & os, DirectoryNode node);
//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
//this version of list files returns in the intial node the files with determi-.
//mate token that existing into the root node directory, if some directory con-.
//tains some target file then the flag is turned to true........................
void list_files_all_tree(DirectoryNode * node, const char * token);

#endif 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif 
